// Deus seja louvado.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <ctype.h>
#include <time.h>


#define NUM_DOCUMENTS 3
#define TAMANHO_MAX_DOC 257

#define TRUE 1
#define FALSE 0

/*Funções principais*/
double TF(int num_ocorrencias, int num_palavras);
double IDF(int num_docs_contendo_termo);

/*Funções de arquivo e buffer*/
int leitor_arquivo(const char *fonte, char buffer[]);
void esvaziar_buffer(char buffer[]);

/*Funções de indentificação dos termos*/
void identificar(const char buffer[], const char termo[], int *n_ocorrencias_termo, int *n_palavras_doc, int *termo_encontrado_no_doc);

int main(int argc, char *argv[]) {
    /*Verifica se o termo foi passado*/
    clock_t inicio, fim;
    double tempo;

    inicio = clock();

    if (argc != 2) {
        printf("Uso: %s <termo>\n", argv[0]);
        return 1;
    }

    const char *termo = argv[1];
    FILE *output;
    /*Variáveis do TF-IDF*/
    int n_ocorrencias_termo[NUM_DOCUMENTS] = {0};
    int n_palavras_doc[NUM_DOCUMENTS] = {0};
    int num_docs_contendo_termo = 0;

    /*Buffers e arquivos*/
    char buffer[TAMANHO_MAX_DOC];
    const char *documentos[NUM_DOCUMENTS] = {"src\\Co-rotinas_256.txt", "src\\Multithreading_256.txt", "src\\Multiprocessamento_256.txt"};

    /*Processamento dos documentos*/
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        esvaziar_buffer(buffer);
        if (leitor_arquivo(documentos[i], buffer) != 0) {
            printf("Erro ao processar o documento: %s\n", documentos[i]);
            return 1;
        }

        identificar(buffer, termo, &n_ocorrencias_termo[i], &n_palavras_doc[i], &num_docs_contendo_termo);
    }

    /*Cálculo e exibição dos resultados*/
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
    fim = clock();
    tempo = ((double) (fim - inicio))/CLOCKS_PER_SEC;

    output = fopen("output_sequencial.txt", "w");

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
    fprintf(output, "Tempo gasto: %f segundos\n", tempo);


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
        buffer[i++] = tolower((char) character);
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
        while (*p == ' ' || *p == '\n') p++; // Ignora espaços ou novas linhas
        if (*p == '\0') break;

        const char *inicio_palavra = p;

        while (*p != ' ' && *p != '\n' && *p != '\0') p++;

        (*n_palavras_doc)++;

        if (strncmp(inicio_palavra, termo, p - inicio_palavra) == 0 && strlen(termo) == (size_t)(p - inicio_palavra)) {
            (*n_ocorrencias_termo)++;
            termo_presente_no_doc = TRUE;
        }
    }

    if (termo_presente_no_doc) {
        (*termo_encontrado_no_doc)++;
    }
}
