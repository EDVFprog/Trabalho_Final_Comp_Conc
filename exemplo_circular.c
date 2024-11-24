#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_PRODUCERS 3
#define BUFFER_SIZE 10

typedef struct {
    int data[BUFFER_SIZE];
    int count;
    sem_t full;  // Semáforo indicando se o buffer está cheio
    sem_t empty; // Semáforo indicando se o buffer está vazio
} Buffer;

Buffer buffers[NUM_PRODUCERS];

// Função do produtor
void* producer(void* arg) {
    int id = *(int*)arg;
    int next = (id + 1) % NUM_PRODUCERS;

    while (1) {
        // Simular produção
        printf("Produtor %d produzindo...\n", id);
        sem_wait(&buffers[id].empty); // Espera o buffer estar vazio

        for (int i = 0; i < BUFFER_SIZE; i++) {
            buffers[id].data[i] = rand() % 100; // Produz dados
        }
        buffers[id].count = BUFFER_SIZE;
        printf("Produtor %d encheu o buffer.\n", id);

        sem_post(&buffers[id].full); // Libera o buffer para o próximo

        // Espera o próximo buffer ser consumido antes de continuar
        sem_wait(&buffers[next].empty);
    }
    return NULL;
}

// Função do consumidor
void* consumer(void* arg) {
    while (1) {
        for (int i = 0; i < NUM_PRODUCERS; i++) {
            sem_wait(&buffers[i].full); // Espera o buffer estar cheio

            // Processa os dados
            printf("Consumidor processando buffer %d: ", i);
            for (int j = 0; j < BUFFER_SIZE; j++) {
                printf("%d ", buffers[i].data[j]);
            }
            printf("\n");

            sem_post(&buffers[i].empty); // Marca o buffer como vazio
        }
    }
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS], consumer_thread;
    int ids[NUM_PRODUCERS];

    // Inicializa os buffers
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        sem_init(&buffers[i].full, 0, 0);
        sem_init(&buffers[i].empty, 0, 1);
        buffers[i].count = 0;
    }

    // Cria os produtores
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &ids[i]);
    }

    // Cria o consumidor
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Junta as threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    pthread_join(consumer_thread, NULL);

    return 0;
}
