#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#define NUM_DOCUMENTS 3
#define TAMANHO_MAX_DOC 1000000

// Struct para armazenar o buffer de um documento
typedef struct {
    char buffer[TAMANHO_MAX_DOC];
    int num_palavras;
} Documento;

// Struct para armazenar as métricas de cada documento
typedef struct {
    int ocorrencias_termo;
    int num_palavras;
} DocumentoMetrica;

// Variáveis globais
Documento documentos[NUM_DOCUMENTS];
DocumentoMetrica metricas[NUM_DOCUMENTS];
const char *termo;
char *palavras[NUM_DOCUMENTS][TAMANHO_MAX_DOC];

void ler_arquivo(const char *arquivo, char *buffer);
void pre_processamento(const char *buffer, char **palavras, int *num_palavras);
void buscar_termo(const char **palavras, int num_palavras, const char *termo, int *ocorrencias);
double TF(int num_ocorrencias, int num_palavras);
double IDF(int num_docs_contendo_termo);
void calcular_metricas(FILE *output);

int main(int argc, char *argv[]) {
    clock_t inicio, fim;
    double tempo;
    inicio = clock();

    if (argc != 2) {
        printf("Uso: %s <termo>\n", argv[0]);
        return 1;
    }

    char *string = argv[1];
    for (int i = 0; string[i]; i++) {
        string[i] = tolower(string[i]);
    }
    termo = string;

    // Nomes dos arquivos de entrada
    const char *documentos_entrada[NUM_DOCUMENTS] = {"Co-rotinas_64.txt", "Multiprocessamento_64.txt", "Multithreading_64.txt"};

    // Leitura e processamento sequencial dos arquivos
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        ler_arquivo(documentos_entrada[i], documentos[i].buffer);
        pre_processamento(documentos[i].buffer, palavras[i], &documentos[i].num_palavras);
        metricas[i].num_palavras = documentos[i].num_palavras;

        // Contar ocorrências do termo no documento atual
        int ocorrencias = 0;
        buscar_termo((const char **)palavras[i], documentos[i].num_palavras, termo, &ocorrencias);
        metricas[i].ocorrencias_termo = ocorrencias;
    }

    // Output
    FILE *output = fopen("Resultados/Conc_N/output_conc_N64_co-rotinas_sequencial.txt", "w");
    if (output == NULL) {
        printf("Erro ao abrir o arquivo de saída!\n");
        return 1;
    }

    calcular_metricas(output);
    fim = clock();
    tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
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

void pre_processamento(const char *buffer, char **palavras, int *num_palavras) {
    char *texto = strdup(buffer);
    char *token = strtok(texto, " \n\t\r");
    *num_palavras = 0;

    while (token != NULL) {
        for (int i = 0; token[i]; i++) {
            token[i] = tolower(token[i]);
        }

        palavras[*num_palavras] = strdup(token);
        (*num_palavras)++;
        token = strtok(NULL, " \n\t\r");
    }

    free(texto);
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
