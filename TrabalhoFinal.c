/* Multiplicacao de matriz-matriz de modo concorrente*/
//Vamos modificar para virar nosso trab final
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"


vou cortar a maioria das coisas e deixar algumas ideias

int nthreads; //numero de threads , podemos deixar fixo e mudar a cada execucao , sem pedir ao usuario

typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int inicio;
   int fim;
} tArgs;  //talvez seja interessante trabalhar com isso

//precisamos criar aqui a funcao que cada thread vai executar
void * tarefa(void *arg) { // ta recebendo como entrada nosso struc
  
   }

//fluxo principal
int main(int argc, char* argv[]) {
   //consertar os prints
   //saber quais variaveis declarar 
   
   long long int tamA,tamB; //qtde de elementos na matriz A e B
   FILE * arqA, *arqB,*arqS; //descritor do arquivos de entrada e arquivo de saída
   int tam = colunasA*linhasB;
   size_t retA,retB,ret; //retorno da funcao de escrita no arquivo de saida
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e dimensao
   double inicio, fim, delta;
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   if(argc<5) {
      printf("Digite: %s <arquivos de entrada> <arquivo de entrada> <arquivo de entrada>  <arquivo de saída> \n", argv[0]);
      //consertar os prints
      return 1;
   }
  

   //abre o arquivo para leitura txt de entrada do arquivo1
   arqA = fopen(argv[1], "rb");
   if(!arqA) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }
   //abre o arquivo para leitura txt de entrada do arquivo2
   arqB = fopen(argv[2], "rb");
   if(!arqB) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }
   //abre o arquivo para leitura txt de entrada do arquivo3

    arqC = fopen(argv[3], "rb");
   if(!arqC) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }



   //Precisamos ler arquivo 1
  
   //Precisamos ler arquivo 2

   //Precisamos ler arquivo 3


   precisamos terminar tempo de processamento
  

   //vai chamar as threads para chamarem a tarefa da thread
   
   GET_TIME(inicio); // incia tempo de processamento
   //alocacao das estruturas que vamos ultilizar
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}   // vamos deixar?
   args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
   if(args==NULL) {puts("ERRO--malloc"); return 2;}
    
   //criacao das threads, vamos ver como vamos fazer a criacao das nossas threads
    for (int i = 0; i < nthreads; i++) {
        (args + i)->id = i;
        (args + i)->inicio = i * pedaco;
        (args + i)->fim = (i + 1) * pedaco;
        if (i == nthreads - 1) {
            (args + i)->fim = linhasA;
        }

        if (pthread_create(tid + i, NULL, mult, (void*)(args + i))) {
            puts("ERRO--pthread_create");
            return 3;
        }
    }

   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }


   GET_TIME(fim)   //termina tempo de processamento
   delta = fim - inicio;
   printf("Tempo de processamento  (nthreads %d): %lf\n", nthreads, delta);  //consertar os prints
   


    GET_TIME(inicio);  //iniciar tempo de finalizacao
   arqS = fopen(argv[5], "wb"); //tem que ver onde vai ta o arquivo que vai ser de sairda
    if(!arqS) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }
   
   //escreve o resultado no arquivo de saida

   //finaliza processos e libera espaço
   GET_TIME(fim) //finaliza o tempo de finalizacao   
   delta = fim - inicio;
   printf("Tempo finalizacao:%lf\n", delta); //consertar os prints

   return 0;
}