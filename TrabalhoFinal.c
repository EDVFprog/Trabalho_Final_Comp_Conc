#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

#define NUM_THREADS 3
#define NUM_DOCUMENTS 3
//#define ARCHIVE_PATH "src\\arquivo"

int n_docs_com_termo = 0;  //Em quantos documentos o termo aparece.
int palavra_pos_global = 0;//Palavra lida
int indice_arquivo = 0;    //Qual arquivo está sendo lido. 

pthread_mutex_t mutex;

// FUNÇÕES AUXILIARES NO NOSSO TRABALHO
double TF(int num_ocorrencias, int num_palavras);
double IDF();

/*
typedef struct{
   int id;
}tArgs;
*/
void *tarefa(void *arg) {
    char buffer[256]; // Nenhuma linha é maior que 256 caracteres

    while (1) {
        pthread_mutex_lock(&mutex);
        int indice_local = indice_arquivo++; // Cada thread obtém um índice único
        pthread_mutex_unlock(&mutex);

        if (fseek(arquivo, indice_local * TAMANHO_LINHA, SEEK_SET) != 0) {
            // Se o deslocamento não for possível, terminamos
            break;
        }

        if (fgets(buffer, sizeof(buffer), arquivo) == NULL) {
            // Verifique EOF
            break;
        }
        
        pthread_mutex_lock(&mutex);
        if (indice_arquivo >= TOTAL_LINHAS) { // Condição para terminar todas as threads
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


int main(int argc, char* argv[]){

   /*Abre os arquivos*/
   /*Quero abrir os arquivos de duas maneiras
   
      - Leitura de um src
      - Através da linha de comando.
    */

    FILE *arquivo[NUM_DOCUMENTS];
    FILE *output;

    pthread_t tid[NUM_THREADS];


    if(argc == 1){   //Abertura padrão, modo exemplo com acesso a src
      int i = 0;
      char nome_arquivo = ARCHIVE_PATH;
      while(i<NUM_DOCUMENTS){
         char caminho[50];
         sprintf(caminho, "src\\arquivo_%d", i);
         arquivo[i] = fopen(caminho, "r");

         if(arquivo[i] == NULL){ //Caso o arquivo não abra.
            printf("ERRO!! -- Impossível abrir arquivo_%d",i);
            return 1;
         }
      }

      

    }
    else if(argc<5){
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
      }
   }
   /*INSTÂNCIANDO AS THREADS*/
   int i = 0;
   while(i < NUM_THREADS){
      if (pthread_create(&tid[i],NULL,tarefa,NULL)){
         printf("ERRO!! pthread_create()\n"); exit(-1);
      }
   }





   /*Instância*/
   for (int t=0; t<NTHREADS; t++) {//Término das threads
      if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
         } 
      }

   return 0; //Programa foi um sucesso!!
}

double TF(int num_ocorrencias, int num_palavras){
   //
   double TF = num_ocorrencias/num_palavras;
   return TF;
}

double IDF(){
   double IDF = log(NUM_DOCUMENTS/n_docs_com_termo);
   return IDF;
}