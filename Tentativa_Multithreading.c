#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define N_PRODUTOR 2
#define N_CONSUMIDOR 2
#define NUM_DOCUMENTS 3
#define BUFFER_SIZE 20

/*Temos buffers complexos, porque há duas categorias de buffers que gerados ao produzir nossos arquivos, os Puros e Mistos.

    Os puros são buffers que contém dados de um único arquivo apenas.
    Eles necessitam de:
        -dados (cadeias de caracteres)
        -file_id (id do arquivo, será único)
        -offset (posição no arquivo)
    Já os mistos, contém dados de vários arquivos.
    Eles necessitam de:
        -dados
        -file_id múltiplo
        -posição em que iremos alterar o documento
        -offset
    
    Para fins de sincronização, os semáforos são instânciados no buffer. a ideia é fazer da sua execução circular:
    b1 -> b2 -> b3 -> b1 -> b2 -> b3 -> b1 -> ...

    Se por acaso tivermos um:
    
    b3 -> b2... enquanto b1 ainda não terminou, teremos um problema. Isso será melhor explicado em Call.
    
*/
typedef struct{

    char dados[BUFFER_SIZE];        //Dados, bem intuitivo
    int tipo_buffer;                // 0 = puro, 1 é misto
    int file_id[NUM_DOCUMENTS] = {0 , 0, 0};    //
    int offset;     //ofsett no arquivo, posição que será acessada no arquivo.

    sem_t full;
    sem_t empty;

}Buffer;




FILE *arquivo[NUM_DOCUMENTS];

/*Variáveis para o TF-IDF!!*/
int num_ocorrencias[NUM_DOCUMENTS];         
int num_palavras[NUM_DOCUMENTS];
int num_docs_contendo_termo;

int tamanho_arquivo[NUM_DOCUMENTS] = {0};   //Variável que vê o tamanho do arquivo. Ela indica também a posição do EOF
int tamanho_total = 0;
//int tamanho_do_buffer = 20;                 //Tamanho da nossa variável de buffer
int arquivo_atual = 0;

float buffers_por_arquivo[NUM_DOCUMENTS] = {0};   //Essa variável indica a quantidade de buffers necessários para consumir um arquivo
float buffers_total = 0;                          //Essa variável indica a quantidade total de buffers necessários para a execução completa.
float fim_de_arquivo[NUM_DOCUMENTS];              //Estamos armazenando em qual buffer o arquivo x irá terminar
int buffers_disparados = 0;
int posicao_atual[N_PRODUTORES] = {0};
Buffer buffer[N_PRODUTOR];   //Nosso buffer

/*Funções para o TF-IDF*/
void *produtor(void *args);
void *consumidor(void *args);
int encher_buffer();
int esvaziar_buffer();
double TF(int num_ocorrencias, int num_palavras);
double IDF(int num_docs_contendo_termo);

/*Sincronização!*/
pthread_mutex_t mutex;

//pthread_mutex_t mutex_2;  
//pthread_cond_t produtor;
//pthread_cond_t consumidor;
int produtores_ativos = N_PRODUTOR; //variável de controle para liberar os consumidores
int consumidores_ativos = 0;        //variável de controle para liberar os produtores




int main(int argc, char* argv[]){




    int num_threads = N_PRODUTOR + N_CONSUMIDOR;    //definindo número de threads
    float buffer_por_arquivo_f[NUM_DOCUMENTS];

   /*Abertura de arquivos*/
    //FILE *arquivo[NUM_DOCUMENTS];   //Para o nosso input
    //Coloquei o arquivo como global, cometerei esse crime.

    FILE *output;                   //Para o nosso output

    pthread_t produtores[N_PRODUTOR], consumidores[N_CONSUMIDOR];
    int ids[N_PRODUTOR];

    //Inicializa os buffers
    for (int i = 0; i < N_PRODUTOR; i++) {
        sem_init(&buffers[i].full, 0, 0);
        sem_init(&buffers[i].empty, 0, 1);
        buffers[i].count = 0;
    }

    if(argc == 1){   //Abertura padrão, ACESSA DIRETAMENTE A SOURCE
      int i = 0;
      //char nome_arquivo = ARCHIVE_PATH;   //Talvez seja implementado depois
      while(i<NUM_DOCUMENTS){   

         char caminho[50];  
         sprintf(caminho, "src\\doc%d.txt", i);//estou escrevendo a string no vetor caminho.
         arquivo[i] = fopen(caminho, "r");

         if(arquivo[i] == NULL){ //Caso o arquivo não abra.
            printf("ERRO!! -- Impossível abrir arquivo_%d",i);
            return 1;
         }

        fseek(arquivo[i],0,SEEK_END);//Põe o ponteiro do arquivo no FIM dele
        tamanho_arquivo[i] = ftell(arquivo[i]);
        tamanho_total += tamanho_arquivo[i];    //soma ao tamanho total.
        fseek(arquivo[i],0,SEEK_SET);//Põe o ponteiro do arquivo no início

        buffers_por_arquivo[i] = tamanho_arquivo/BUFFER_SIZE; //Isso é um float!!
        buffers_total += buffers_por_arquivo[i];            //Isso é outro float!!
        fim_de_arquivo[i] = buffers_total;                  //Nossa array de fins de arquivo

        //Estamos considerando floats porque nossos buffers irão consumir partes de outros arquivos também.
        /*
            Suponha que eu tenha um BUFFER_SIZE de 10, com 2 arquivos com 45 caracteres.
            
            Se eu utilizasse um int para calcular o número de buffers necessários para cada arquivo, teria
            a quantidade de 5 buffers disparados para cada, pois 45/10 = 4.5 que arredondando para cima é 5.

            No total, teriamos 10 disparos para consumir o arquivo.

            Mas como usamos float, somamos 4.5 com 4.5 e temos 9, totalizando 9 disparos apenas.

            Outro exemplo: se  tivessemos um buffer de tamanho 100 e 3 arquivos de tamanho 30.
            Teriamos 30/100 = 0.3, somando os 3, teriamos 0.9 disparos, o que arredondando é 1.
            Se usassemos int e arredondassemos para cima, teriamos 3 disparos, consideravelmente mais do que o necessário
            
        
        */
        i++;
      }
    }


    else if(argc<5){    /*ERRO DE PASSAGEM DE ARGUMENTOS! */
      printf("Digite: %s <arquivos de entrada> <arquivo de entrada> <arquivo de entrada>  <arquivo de saída> \n", argv[0]);
      //consertar os prints
         return 2;
   }
   else{
      int i = 0;
      while(i<NUM_DOCUMENTS){
         arquivo[i] = fopen(argv[i], "r");

         if(arquivo[i] == NULL){
            printf("ERRO!! -- Impossível abrir arquivo %d",i);
            return 3;
         }
         i++;
      }
   }

    /*Já lido os documentos*/

    buffers_por_arquivo = ()

    /*DISPARANDO THREADS*/

    //Disparo dos produtores
    for(int j=0; j<N_PRODUTOR; j++){    
        if(pthread_create(&tid[j],NULL,produtor,NULL) !=){
            printf("ERRO!!!! Erro ao criar Thread PRODUTOR!!\n");
            return 3;
        }
    }

    //Disparo dos consumidores.
    for(j=0;j<N_CONSUMIDOR;j++){
        if(pthread_create(&tid[N_PRODUTOR + j],consumidor,NULL)){
            printf("ERRO!!!! Erro ao criar Thread CONSUMIDOR!!\n");
            return 3;
        }
    }

    for (int t=0; t<num_threads; t++) {//Término das threads
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); 
            return 4;; 
        } 
    }

    return 0;
}

/*
float buffers_por_arquivo[NUM_DOCUMENTS] = {0};   //Essa variável indica a quantidade de buffers necessários para consumir um arquivo
float buffers_total = 0;                          //Essa variável indica a quantidade total de buffers necessários para a execução completa.
int buffers_disparados = 0;
int posicao_atual[N_PRODUTORES] = {0};
Buffer buffer[N_PRODUTOR];   //Nosso buffer
 */
/*
typedef struct{

    char dados[BUFFER_SIZE];        //Dados, bem intuitivo
    int tipo_buffer;                // 0 = puro, 1 é misto
    int file_id[NUM_DOCUMENTS] = {0 , 0, 0};    //
    int offset;     //ofsett no arquivo, posição que será acessada no arquivo.

    sem_t full;
    sem_t empty;

}Buffer;*/

void *produtor(void *args){
    int id = *(*int) args;                  //ID do consumidor
    int next_id = (id + 1) % N_PRODUTOR;    //ID da próxima thread disparada
    int misto = 0;
    int document_id = 0;
    int buffer_atual;
    /*
        A missão do produtor é encher o buffer com dados do arquivo e mandar para o consumidor.

        Nossos desafios são:
            1 - diminuir custos de I/O
            2 - realizar a troca de maneira circular

    */
    while(1){//Ciclo de produção

        

        pthread_mutex_lock(&mutex);
        

        //Primeira atitude, ver se já disparamos todos os buffers necessários para a leitura do arquivo
        if(buffers_disparados >= buffers_total) break;  //Saímos do ciclo.

        //Segunda atitude, se temos que disparar buffers, em qual buffer estamos?
        buffer_atual = buffers_disparados;

        //Terceira atitude, identificar em qual documento estamos...
        for(int j = 0; j<NUM_DOCUMENTS;j++){
            if(buffer_atual > fim_de_arquivo[j]) document_id++;
        }


        //E por último, identificar se estamos em um buffer misto ou não...
        for(int j = 0; j<NUM_DOCUMENTS; j++){
            if( && (buffer_atual == ((int) fim_de_arquivo[i] + 1))) misto += 1;
        }
        if(misto){
            for(int j = 0; j < misto; j++){
                buffer[id].file_id
            }
            
        }


        buffers_disparados++;
        pthread_mutex_unlock(&mutex);


    }

    pthread_exit(NULL);
    


}

void *consumidor(void *args){
    /*
        O consumidor consome um buffer criado pelo produtor.

        Ele recebe, e identifica a palavra
    */
    

    identificar();
    esvaziar_buffer();


}