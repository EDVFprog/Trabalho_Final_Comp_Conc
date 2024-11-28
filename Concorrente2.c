/*
    A professora Silvana nos orientou a testar diferentes abordagens, e esta foi a melhor.

    Este código implementa um pré-processamento do texto, colocando palavras em
    "Bolsas" para contagem de maneira paralela.
    É uma diferente abordagem para analisar uma cadeia de caracteres. A simples divisão de regiões de um buffer
    acaba se tornando muito complicada por trabalhar muito com expressões regulares.

    basicamente tokenizamos o código antes e depois contamos de maneira paralela.

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#define NUM_DOCUMENTS 3
#define TAMANHO_MAX_DOC 1000000

#define TRUE 1
#define FALSE 0

// struct para armazenar o buffer de um documento
typedef struct {
    char buffer[TAMANHO_MAX_DOC];
    int num_palavras;
} Documento;

// struct para armazenar as métricas de cada documento
typedef struct {
    int ocorrencias_termo;
    int num_palavras;
} DocumentoMetrica;

// Variáveis globais
Documento documentos[NUM_DOCUMENTS];
DocumentoMetrica metricas[NUM_DOCUMENTS];
const char *termo;
char *palavras[NUM_DOCUMENTS][TAMANHO_MAX_DOC];  

// Nosso mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  //Inicializado com um Macro


void ler_arquivo(const char *arquivo, char *buffer);
void pre_processamento(const char *buffer, char **palavras, int *num_palavras);
void buscar_termo(const char **palavras, int num_palavras, const char *termo, int *ocorrencias);
double TF(int num_ocorrencias, int num_palavras);
double IDF(int num_docs_contendo_termo);
void calcular_metricas(FILE *output);

// Função das threads
void *contar_ocorrencias(void *arg);

int main(int argc, char *argv[]) {
    clock_t inicio, fim;
    double tempo;
     inicio = clock();
    if (argc != 2) {
        printf("Uso: %s <termo>\n", argv[0]);
        return 1;
    }

    
   char  * string = argv[1];
   
     for(int i = 0; string[i]; i++){
      string[i] = tolower(string[i]);   //Convertendo o input para letras minúsculas
  
  }
  termo = string;

    // Nomes dos arquivos de entrada
    const char *documentos_entrada[NUM_DOCUMENTS] = {"Co-rotinas_256.txt", "Multiprocessamento_256.txt", "Multithreading_256.txt"};

    // Leitura sequencial dos arquivos
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        ler_arquivo(documentos_entrada[i], documentos[i].buffer);
        pre_processamento(documentos[i].buffer, palavras[i], &documentos[i].num_palavras);
        metricas[i].num_palavras = documentos[i].num_palavras;
    }

    // O número de threads será igual ao número de documentos

    //Teremos uma relação de 1 para 1
    pthread_t threads[NUM_DOCUMENTS];


    //Disparo das threads "contar_ocorrencias"
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        pthread_create(&threads[i], NULL, contar_ocorrencias, (void *)(long)i);
    }

    
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        pthread_join(threads[i], NULL);
    }

    //Output
    FILE *output = fopen("output_conc_N64_co-rotinas.txt", "w");
    if (output == NULL) {
        printf("Erro ao abrir o arquivo de saída!\n");
        return 1;
    }

    calcular_metricas(output);
    fim = clock();
    tempo = ((double) (fim - inicio))/CLOCKS_PER_SEC;
    fprintf(output, "Tempo gasto: %f segundos\n", tempo);
    fclose(output);
   
    return 0;
}


void ler_arquivo(const char *arquivo, char *buffer) {
    FILE *f = fopen(arquivo, "r");
    if (f == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", arquivo);
        exit(1);
    }

    int i = 0;
    int c;
    while ((c = fgetc(f)) != EOF && i < TAMANHO_MAX_DOC - 1) {
        buffer[i++] = (char)c;
    }
    buffer[i] = '\0';
    fclose(f);
}

void pre_processamento(const char *buffer, char **palavras, int *num_palavras) { // Separa as palavras da string.
    char *texto = strdup(buffer);  // Copiar o conteúdo do buffer
    char *token = strtok(texto, " \n\t\r");  // Tokenizar com base em delimitadores (sem pontuação)
    *num_palavras = 0;

    while (token != NULL) {
        for (int i = 0; token[i]; i++) {
            token[i] = tolower(token[i]);   //Palavra para minúscula
        }

        palavras[*num_palavras] = strdup(token);  
        (*num_palavras)++;
        token = strtok(NULL, " \n\t\r"); 
    }

    free(texto);  
}

void *contar_ocorrencias(void *arg) {
    long thread_id = (long)arg;

    int doc_id = thread_id;

    int ocorrencias = 0;
    buscar_termo((const char **)palavras[doc_id], documentos[doc_id].num_palavras, termo, &ocorrencias);

    pthread_mutex_lock(&mutex);
    metricas[doc_id].ocorrencias_termo = ocorrencias;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void buscar_termo(const char **palavras, int num_palavras, const char *termo, int *ocorrencias) {
    for (int i = 0; i < num_palavras; i++) {
        if (strcmp(palavras[i], termo) == 0) {
            (*ocorrencias)++;
        }
    }
}

void calcular_metricas(FILE *output) {
    int num_docs_contendo_termo_local = 0;

    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        if (metricas[i].ocorrencias_termo > 0) {
            num_docs_contendo_termo_local++;
        }
    }

    double idf = IDF(num_docs_contendo_termo_local);  
     fprintf(output, "IDF: %.4f\n", idf);
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        double tf = TF(metricas[i].ocorrencias_termo, metricas[i].num_palavras);
        double tf_idf = tf * idf;

        fprintf(output, "Documento %d: TF = %.4f\n", i + 1, tf);
        fprintf(output, "Documento %d: TF-IDF = %.4f\n", i + 1, tf_idf);
       
    }
}

double TF(int num_ocorrencias, int num_palavras) {
    return (double)num_ocorrencias / num_palavras;
}

double IDF(int num_docs_contendo_termo) {
    if (num_docs_contendo_termo == 0) {
        return 0.0;
    }
    return log10((double)NUM_DOCUMENTS / num_docs_contendo_termo);
}

