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

char buffer[TAMANHO_MAX_DOC];


int main(int argc, char argv[]){
    /*Variáveis do TF-IDF*/
    int n_ocorrencias_termo[NUM_DOCUMENTS];
    int n_palavras_doc[NUM_DOCUMENTS];
    //int num_docs;
    int num_docs_contendo_termo[NUM_DOCUMENTS];
    
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
void identificar(char termo[],int indice_doc, int &n_ocorrencias_termo, int &n_palavras_doc, int &num_docs_contendo_termo){
    //Função está parametrizada por referência.
    /*Essa função deve:
        1 - percorrer o buffer.
        2 - identificar as palavras do buffer
            2.1 se a palavra identificada for a mesma do nosso termo, incrementa o valor de ocorrência,nº total de palavras
                    e indique que aquele doc tem aquela palavra presente em seu texto.
            2.2 se não for o termo, então incrementa o nº total de termos apenas
    */
    
    /*Como percorrer o buffer????

        primeiramente faço um buffer local
    
        Meu critério é:
            caracter é '\0', termina.
            caracter é {' ', '\n'}? Pula.
            caracter é letra e\ou numero?
                -letra e\ou número faz parte do termo?
                    SE SIM:
                        É FINAL DO TERMO?
                            Verifica se o próximo character após o termo é {' ', '\n', '\0'}
                            SE SIM:
                                +INCREMENTA A OCORRÊNCIA DO TERMO
                                +INCREMENTA O NUMERO DE PALAVRAS DO DOC
                                +INCREMENTA O NUM DE DOCS CONTENDO O TERMO
                            SE NÃO:                                      
                                +INCREMENTA O PONTEIRO E VERIFICA A PRÓXIMA LETRA.
                    SE NÃO:
                        +INCREMENTA O NUMERO DE PALAVRAS DO DOC
                        ->Faça um loop até um espaço em branco, nova linha ou fim do documento.
    */

    char *p;    //Nosso ponteiro para percorrer o buffer
    p = buffer; //Nosso ponteiro aponta para o primeiro elemento do buffer
    int i = 0;  //Um índice

    while(*p != '\0'){
        i = 0;

        if(*p == ' ' || *p == '\n'){/*Saltei*/
            p++;
            continue;
        }
        else if(*p == termo[i]){//Para chegar no else, não temos nem EOF, nem espaço, nem nova linha...
            while(*p == termo[i]){
                i++;
                p++;
                //Percorre até o fim da palavra
            }
            i = 0;//Resetando o indice
            if(*p == ' ' || *p == '\n' || *p == '\0'){//se a próxima letra for um fim... Então encontramos um termo
                *n_ocorrencias_termo[indice_doc] += 1;
                *n_palavras_doc[indice_doc] +=1;
                *num_docs_contendo_termo[indice_doc] += 1;
            }
            else{//Se não for... então não era a palavra.
                *n_palavras_doc[indice_doc] += 1;
                while(*p != ' ' || *p != '\n' || *p != '\0') p++;   //Percorre até a próxima palavra ou fim.
            }

        }



        p++;
    }

}