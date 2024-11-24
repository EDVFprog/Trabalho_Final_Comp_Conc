// Deus seja louvado.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#define NUM_DOCUMENTS 3
#define TAMANHO_MAX_DOC 257

#define TRUE 1
#define FALSE 0

/* Estrutura para o buffer compartilhado */
typedef struct {
    char buffer[TAMANHO_MAX_DOC];
    int doc_id;
    int ocupado; // Flag para indicar se o buffer está ocupado
} BufferCompartilhado;

/* Variáveis globais */
BufferCompartilhado buffer_compartilhado;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;

const char *documentos[NUM_DOCUMENTS] = {"doc1.txt", "doc2.txt", "doc3.txt"};
const char *termo;

/* Resultados globais */
int n_ocorrencias_termo[NUM_DOCUMENTS] = {0};
int n_palavras_doc[NUM_DOCUMENTS] = {0};
int num_docs_contendo_termo = 0;

/* Funções principais */
double TF(int num_ocorrencias, int num_palavras);
double IDF(int num_docs_contendo_termo);
int leitor_arquivo(const char *fonte, char buffer[]);
void esvaziar_buffer(char buffer[]);
void identificar(const char buffer[], const char termo[], int *n_ocorrencias_termo, int *n_palavras_doc, int *termo_encontrado_no_doc);

/* Threads */
void *produtor(void *arg);
void *consumidor(void *arg);

int main(int argc, char *argv[]) {
   if (argc != 2) {
      printf("Uso: %s <termo>\n", argv[0]);
      return 1;
   }

   termo = argv[1];
   pthread_t thread_produtor, thread_consumidor;

   buffer_compartilhado.ocupado = FALSE;

   FILE *output;

    /* Criação das threads */
   pthread_create(&thread_produtor, NULL, produtor, NULL);
   pthread_create(&thread_consumidor, NULL, consumidor, NULL);

    /* Aguardar as threads terminarem */
   pthread_join(thread_produtor, NULL);
   pthread_join(thread_consumidor, NULL);

    /* Cálculo e exibição dos resultados */
    /*
    printf("Termo: %s\n", termo);
    printf("Resultados:\n");
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        double tf = TF(n_ocorrencias_termo[i], n_palavras_doc[i]);
        printf("Documento %d: TF = %.4f\n", i + 1, tf);
    }
    double idf = IDF(num_docs_contendo_termo);
    printf("IDF: %.4f\n", idf);
   */
   output = fopen("output.txt", "w");

   if(output == NULL){
      printf("ERRO NA CONSTRUÇÃO DO OUTPUT\n");
      return 404;
   }
   for (int i = 0; i < NUM_DOCUMENTS; i++) {
        double tf = TF(n_ocorrencias_termo[i], n_palavras_doc[i]);
        fprintf(output,"Documento %d: TF = %.4f\n", i + 1, tf);
    }
    double idf = IDF(num_docs_contendo_termo);
    fprintf(output,"IDF: %.4f\n", idf);

    fclose(output);
   return 0;
}

double TF(int num_ocorrencias, int num_palavras) {
    return (double) num_ocorrencias / num_palavras;
}

double IDF(int num_docs_contendo_termo) {
    return log((double) NUM_DOCUMENTS / num_docs_contendo_termo);
}

int leitor_arquivo(const char *fonte, char buffer[]) {
    FILE *arquivo = fopen(fonte, "r");
    if (arquivo == NULL) {
        printf("ERRO!!! -- Não foi possível abrir o arquivo: %s\n", fonte);
        return 1;
    }
    int i = 0, character;
    while ((character = fgetc(arquivo)) != EOF) {
        buffer[i++] = (char) character;
        if (i >= TAMANHO_MAX_DOC - 1) break; // Evita buffer overflow
    }
    buffer[i] = '\0';
    fclose(arquivo);
    return 0;
}

void esvaziar_buffer(char buffer[]) {
    for (int i = 0; i < TAMANHO_MAX_DOC; i++) {
        buffer[i] = '\0';
    }
}

void identificar(const char buffer[], const char termo[], int *n_ocorrencias_termo, int *n_palavras_doc, int *termo_encontrado_no_doc) {
    int termo_presente_no_doc = FALSE;
    const char *p = buffer;

    while (*p != '\0') {
        // Ignora espaços e quebras de linha
        while (*p == ' ' || *p == '\n') p++; 
        if (*p == '\0') break;

        // Localiza o início da palavra
        const char *inicio_palavra = p;

        // Avança até o final da palavra
        while (*p != ' ' && *p != '\n' && *p != '\0') p++;

        // Atualiza a contagem de palavras
        (*n_palavras_doc)++;

        // Verifica se a palavra é o termo procurado
        if (strncmp(inicio_palavra, termo, p - inicio_palavra) == 0 && (p - inicio_palavra) == strlen(termo)) {
            (*n_ocorrencias_termo)++;
            termo_presente_no_doc = TRUE;
        }
    }

    // Atualiza se o termo foi encontrado no documento
    *termo_encontrado_no_doc += termo_presente_no_doc;
}

void *produtor(void *arg) {
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        pthread_mutex_lock(&mutex);
        while (buffer_compartilhado.ocupado) {
            pthread_cond_wait(&cond_producer, &mutex);
        }

        esvaziar_buffer(buffer_compartilhado.buffer);
        if (leitor_arquivo(documentos[i], buffer_compartilhado.buffer) != 0) {
            printf("Erro ao processar o documento: %s\n", documentos[i]);
        }
        buffer_compartilhado.doc_id = i;
        buffer_compartilhado.ocupado = TRUE;

        pthread_cond_signal(&cond_consumer);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *consumidor(void *arg) {
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        pthread_mutex_lock(&mutex);
        while (!buffer_compartilhado.ocupado) {
            pthread_cond_wait(&cond_consumer, &mutex);
        }

        identificar(buffer_compartilhado.buffer, termo, &n_ocorrencias_termo[buffer_compartilhado.doc_id], &n_palavras_doc[buffer_compartilhado.doc_id], &num_docs_contendo_termo);

        buffer_compartilhado.ocupado = FALSE;

        pthread_cond_signal(&cond_producer);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
