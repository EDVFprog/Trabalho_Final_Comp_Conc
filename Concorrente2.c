#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#define NUM_DOCUMENTS 3
#define TAMANHO_MAX_DOC 1000000
#define MAX_THREADS 3  // Número de threads para contagem de ocorrências

#define TRUE 1
#define FALSE 0

// Estrutura para armazenar o buffer de um documento
typedef struct {
    char buffer[TAMANHO_MAX_DOC];
    int num_palavras;
} Documento;

// Estrutura para armazenar as métricas de cada documento
typedef struct {
    int ocorrencias_termo;
    int num_palavras;
} DocumentoMetrica;

// Variáveis globais
Documento documentos[NUM_DOCUMENTS];
DocumentoMetrica metricas[NUM_DOCUMENTS];
const char *termo;
char *palavras[NUM_DOCUMENTS][TAMANHO_MAX_DOC];  // Vetor de palavras extraídas dos documentos

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Funções principais
void ler_arquivo(const char *arquivo, char *buffer);
void tokenizar_e_lowercase(const char *buffer, char **palavras, int *num_palavras);
void buscar_termo(const char **palavras, int num_palavras, const char *termo, int *ocorrencias);
double TF(int num_ocorrencias, int num_palavras);
double IDF(int num_docs_contendo_termo);
void calcular_metricas(FILE *output);

// Funções das threads
void *contar_ocorrencias(void *arg);

int main(int argc, char *argv[]) {
    clock_t inicio, fim;
    double tempo;
     inicio = clock();
    if (argc != 2) {
        printf("Uso: %s <termo>\n", argv[0]);
        return 1;
    }

    // Armazenar o termo em minúsculas
   char  * string = argv[1];
     for(int i = 0; string[i]; i++){
      string[i] = tolower(string[i]);
  
  }
  termo = string;

    // Nomes dos arquivos de entrada
    const char *documentos_entrada[NUM_DOCUMENTS] = {"Co-rotinas_64.txt", "Multiprocessamento_64.txt", "Multithreading_64.txt"};

    // Leitura sequencial dos arquivos
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        ler_arquivo(documentos_entrada[i], documentos[i].buffer);
        tokenizar_e_lowercase(documentos[i].buffer, palavras[i], &documentos[i].num_palavras);
        metricas[i].num_palavras = documentos[i].num_palavras;
    }

    // Threads para contar as ocorrências do termo
    int num_threads =MAX_THREADS;  // O número de threads será igual ao número de documentos
    pthread_t threads[num_threads];

    // Criar uma thread para cada documento
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, contar_ocorrencias, (void *)(long)i);
    }

    // Aguardar as threads de contagem
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Calcular e exibir as métricas de TF, IDF e TF-IDF
    FILE *output = fopen("Resultados/Conc_N/output_conc_N64_co-rotinas.txt", "w");
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

// Função para ler o conteúdo de um arquivo
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

// Função de tokenização e conversão para minúsculas
void tokenizar_e_lowercase(const char *buffer, char **palavras, int *num_palavras) {
    char *texto = strdup(buffer);  // Copiar o conteúdo do buffer
    char *token = strtok(texto, " \n\t\r");  // Tokenizar com base em delimitadores (sem pontuação)
    *num_palavras = 0;

    while (token != NULL) {
        // Convertendo a palavra para minúscula
        for (int i = 0; token[i]; i++) {
            token[i] = tolower(token[i]);
        }

        palavras[*num_palavras] = strdup(token);  // Armazenar a palavra no vetor
        (*num_palavras)++;
        token = strtok(NULL, " \n\t\r");  // Continuar tokenizando
    }

    free(texto);  // Liberar a memória alocada para o texto
}

// Função para contar as ocorrências do termo
void *contar_ocorrencias(void *arg) {
    long thread_id = (long)arg;

    // Cada thread processa um documento específico
    int doc_id = thread_id;

    int ocorrencias = 0;
    buscar_termo((const char **)palavras[doc_id], documentos[doc_id].num_palavras, termo, &ocorrencias);

    // Atualiza as métricas com o número de ocorrências
    pthread_mutex_lock(&mutex);
    metricas[doc_id].ocorrencias_termo = ocorrencias;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

// Função para buscar o termo e contar as ocorrências
void buscar_termo(const char **palavras, int num_palavras, const char *termo, int *ocorrencias) {
    for (int i = 0; i < num_palavras; i++) {
        if (strcmp(palavras[i], termo) == 0) {
            (*ocorrencias)++;
        }
    }
}

// Função para calcular as métricas TF, IDF e TF-IDF
void calcular_metricas(FILE *output) {
    int num_docs_contendo_termo_local = 0;

    // Contar quantos documentos contêm o termo
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        if (metricas[i].ocorrencias_termo > 0) {
            num_docs_contendo_termo_local++;
        }
    }

    // Calcular e exibir as métricas para cada documento
    double idf = IDF(num_docs_contendo_termo_local);  // Calcular o IDF uma vez
     fprintf(output, "IDF: %.4f\n", idf);
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        double tf = TF(metricas[i].ocorrencias_termo, metricas[i].num_palavras);
        double tf_idf = tf * idf;

        fprintf(output, "Documento %d: TF = %.4f\n", i + 1, tf);
        fprintf(output, "Documento %d: TF-IDF = %.4f\n", i + 1, tf_idf);
       
    }
}

// Função para calcular TF (Term Frequency)
double TF(int num_ocorrencias, int num_palavras) {
    return (double)num_ocorrencias / num_palavras;
}

// Função para calcular IDF (Inverse Document Frequency)
double IDF(int num_docs_contendo_termo) {
    if (num_docs_contendo_termo == 0) {
        return 0.0;
    }
    return log10((double)NUM_DOCUMENTS / num_docs_contendo_termo);
}

