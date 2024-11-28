/*
    Esta é nossa implementação sequencial do algoritmo do TF-IDF.
    O algoritmo consiste em contar palavras em documentos, destacando termos específicos e
    incrementando algumas métricas. O objetivo é identificar se um termo define e diferencia um documento
    em relação a outros.

    O código recebe um arquivo, Põe em um buffer e lê caracter a caracter, em busca de palavras.
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

#define TRUE 1          //Definindo somente por praticidade
#define FALSE 0

//Funções do algoritmo
double TF(int num_ocorrencias, int num_palavras);
double IDF(int num_docs_contendo_termo);

//manipulando a memória
int leitor_arquivo(const char *fonte, char buffer[]);
void esvaziar_buffer(char buffer[]);

//Função que identifica as palavras.
void identificar(const char buffer[], const char termo[], int *n_ocorrencias_termo, int *n_palavras_doc, int *termo_encontrado_no_doc);

int main(int argc, char *argv[]) {
    
    
    clock_t inicio, fim;
    double tempo;

    inicio = clock();

    if (argc != 2) {
        printf("Uso: %s <termo>\n", argv[0]);
        return 1;
    }

     char *termo = argv[1];
 
 // Convertendo para lowercase
    for (int i = 0; termo[i] != '\0'; i++) {
        termo[i] = tolower(termo[i]);
    }

    FILE *output;
    /*Variáveis do TF-IDF*/
    int n_ocorrencias_termo[NUM_DOCUMENTS] = {0};
    int n_palavras_doc[NUM_DOCUMENTS] = {0};
    int num_docs_contendo_termo = 0;

    //buffer e caminhos para arquivos.
    char buffer[TAMANHO_MAX_DOC];
    const char *documentos[NUM_DOCUMENTS] = {"doc1.txt", "doc2.txt", "doc3.txt"};

    //Processamento dos documento

    /*
        Nesta ordem:
            -Esvaziamos o buffer.
            -Enchemos o buffer lendo o arquivo.
            -identificamos as palavras.
    */
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        esvaziar_buffer(buffer);
        if (leitor_arquivo(documentos[i], buffer) != 0) {
            printf("Erro ao processar o documento: %s\n", documentos[i]);
            return 1;
        }

        identificar(buffer, termo, &n_ocorrencias_termo[i], &n_palavras_doc[i], &num_docs_contendo_termo);
    }

  
    //Nossa saída
    output = fopen("output_sequencial.txt", "w");

    if(output == NULL){
        printf("ERRO NA CONSTRUÇÃO DO OUTPUT\n");
        return 404;
    }
    double idf = IDF(num_docs_contendo_termo);
    fprintf(output,"IDF: %.4f\n", idf);
    for (int i = 0; i < NUM_DOCUMENTS; i++) {
        double tf = TF(n_ocorrencias_termo[i], n_palavras_doc[i]);
        double tf_idf = idf*tf;
        fprintf(output,"Documento %d: TF = %.4f\n", i + 1, tf);
        fprintf(output,"Documento %d: TF-IDF = %.4f\n", i + 1, tf_idf);
    }
   
    fim = clock();
    tempo = ((double) (fim - inicio))/CLOCKS_PER_SEC;
    fprintf(output, "Tempo gasto: %f segundos\n", tempo);
    fclose(output);
    
    return 0;
}

//Term Frequency
double TF(int num_ocorrencias, int num_palavras) {
    return (double) num_ocorrencias / num_palavras;
}
//
double IDF(int num_docs_contendo_termo) {
    if (num_docs_contendo_termo==0){
    	return 0.0;
    }
    else{
    return log10((double) NUM_DOCUMENTS / num_docs_contendo_termo);}
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
