// Deus seja louvado.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

#define NUM_DOCUMENTS 3
#define TAMANHO_MAX_DOC 257

#define TRUE 1
#define FALSE 0

/*Funções principais*/
double TF(int num_ocorrencias, int num_palavras);
double IDF();

/*Funções de arquivo e buffer*/
int leitor_arquivo(FILE *arquivo, char fonte[])
int esvaziar_buffer();

/*Funções de indentificação dos termos*/
void identificar(); //essa tarefa será única no caso sequencial. Porém deverá ser dividida na concorrência.




int main(int argc, char argv[]){
    /*Variáveis do TF-IDF*/
    int n_ocorrencias_termo[NUM_DOCUMENTS];
    int n_palavras_doc[NUM_DOCUMENTS];
    //int num_docs;
    int num_docs_contendo_termo[NUM_DOCUMENTS];

    char buffer[TAMANHO_MAX_DOC];
    FILE *arquivo[NUM_DOCUMENTS];
    int i = 0;
    
    
    char ch;

    


    return 0;
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


int leitor_arquivo(FILE *arquivo, char fonte[]){
    int i = 0; //índice do buffer;
    int character; 
    char ch;    //variável auxiliar para ler os caracteres do arquivo.

    /*
    Existe uma treta no fgetc que ele interpreta os inteiros como sendo int, isso se deve ao fato
    do EOF ser um inteiro representado por -1.

    Então faço duas variáveis para leitura, uma inteira e uma char, e depois dou cast.
    */

    
    arquivo = fopen(fonte, "r");    //Abertura de arquivo só para leitura.
    if(arquivo == NULL){
        printf("ERRO!!! -- Não foi possível abrir o arquivo");
        return 1;
    }

    while((character = fgetc(arquivo)) != EOF){
        ch = (char) character;
        buffer[i] = ch;
        i++;
    }
    buffer[i] = '\0'; //termino o arquivo com o EOF que não seria incluído.
    fclose(arquivo);
    
    return 0;
}

int esvaziar_buffer(){
    int i = 0;
    while(i < TAMANHO_MAX_DOC){
        //buffer[i] = ' ';
        buffer[i] = '\0';   //Julguei muito mais inteligente encher o buffer de EOF, pois é condição de parada 
                            //da nossa leitura do buffer. O EOF é um indicador que não existe mais nada lá para
                            //ser explorado.
        i++;
    }
}

void identificar(const char buffer[], const char termo[], int indice_doc, 
                 int &n_ocorrencias_termo, int &n_palavras_doc, int &num_docs_contendo_termo) {
                    
    bool termo_encontrado_no_doc = false;
    const char *p = buffer;
    int i = 0;
    n_palavras_doc = 0;
    n_ocorrencias_termo = 0;

    while (*p != '\0') {
        // Ignora espaços ou novas linhas
        while (*p == ' ' || *p == '\n') p++;

        if (*p == '\0') break; // Fim do buffer

        const char *inicio_palavra = p;

        // Avança ponteiro até o fim da palavra
        while (*p != ' ' && *p != '\n' && *p != '\0') p++;

        n_palavras_doc++;

        // Compara a palavra extraída com o termo
        if (strncmp(inicio_palavra, termo, p - inicio_palavra) == 0 && strlen(termo) == size_t(p - inicio_palavra)) {
            n_ocorrencias_termo++;
            termo_encontrado_no_doc = true;
        }
    }

    // Atualiza o número de documentos contendo o termo
    if (termo_encontrado_no_doc) {
        num_docs_contendo_termo++;
    }
}
