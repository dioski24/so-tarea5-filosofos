/*
 * Tarea 5 - La cena de los filosofos
 * Solucion con arreglo de estados y un semaforo por filosofo
 * (diapositivas 45-47): un filosofo solo come si ninguno de sus
 * dos vecinos esta comiendo. Evita el bloqueo mutuo (deadlock).
 */
#include <stdio.h>       /* printf, fprintf */
#include <stdlib.h>      /* exit, EXIT_FAILURE, rand_r */
#include <unistd.h>      /* usleep */
#include <time.h>        /* time */
#include <pthread.h>     /* pthread_* */
#include <semaphore.h>   /* sem_* */

#define SHARED      0
#define N           5            /* numero de filosofos (y de tenedores) */
#define COMIDAS     3            /* veces que come cada filosofo         */

#define PENSANDO    0
#define HAMBRIENTO  1
#define COMIENDO    2

#define IZQ(i) (((i) + N - 1) % N)   /* vecino de la izquierda */
#define DER(i) (((i) + 1) % N)       /* vecino de la derecha   */

int estado[N];                   /* estado de cada filosofo            */
sem_t mutex;                     /* protege el arreglo estado          */
sem_t s[N];                      /* un semaforo por filosofo (inicia 0) */

/* Imprime el estado de toda la mesa: P=pensando H=hambriento C=comiendo.
   Siempre se llama con mutex tomado. */
void imprimir_mesa(void)
{
    const char letra[] = {'P', 'H', 'C'};
    int i;
    printf("   Mesa: [");
    for (i = 0; i < N; i++) {
        printf(" %c", letra[estado[i]]);
    }
    printf(" ]\n");
}

/* Si i tiene hambre y sus vecinos no comen, i pasa a comer */
void test(int i)
{
    if (estado[i] == HAMBRIENTO &&
        estado[IZQ(i)] != COMIENDO &&
        estado[DER(i)] != COMIENDO) {
        estado[i] = COMIENDO;
        printf("Filosofo %d toma los tenedores %d y %d y COME\n", i, i, DER(i));
        imprimir_mesa();
        sem_post(&s[i]);                 /* V(s[i]): despierta a i */
    }
}

void tomar_tenedores(int i)
{
    sem_wait(&mutex);                    /* P(mutex) */
    estado[i] = HAMBRIENTO;
    printf("Filosofo %d tiene HAMBRE\n", i);
    imprimir_mesa();
    test(i);                             /* intenta tomar ambos tenedores */
    sem_post(&mutex);                    /* V(mutex) */
    sem_wait(&s[i]);                     /* P(s[i]): se bloquea si no pudo comer */
}

void dejar_tenedores(int i)
{
    sem_wait(&mutex);                    /* P(mutex) */
    estado[i] = PENSANDO;
    printf("Filosofo %d deja los tenedores y PIENSA\n", i);
    imprimir_mesa();
    test(IZQ(i));                        /* el vecino izquierdo puede comer? */
    test(DER(i));                        /* el vecino derecho puede comer?   */
    sem_post(&mutex);                    /* V(mutex) */
}

void *filosofo(void *arg)
{
    int i = *(int *)arg;
    unsigned int semilla = (unsigned int)time(NULL) ^ (i * 7919);
    int k;

    for (k = 1; k <= COMIDAS; k++) {
        usleep(rand_r(&semilla) % 200000);   /* think: 0-200 ms */
        tomar_tenedores(i);
        usleep(rand_r(&semilla) % 200000);   /* eat:   0-200 ms */
        dejar_tenedores(i);
    }

    sem_wait(&mutex);
    printf("Filosofo %d termino sus %d comidas\n", i, COMIDAS);
    sem_post(&mutex);
    pthread_exit(NULL);
}

int main(void)
{
    pthread_attr_t tattr;
    pthread_t tid[N];
    int ids[N];
    int i;

    sem_init(&mutex, SHARED, 1);
    for (i = 0; i < N; i++) {
        estado[i] = PENSANDO;
        sem_init(&s[i], SHARED, 0);
    }

    pthread_attr_init(&tattr);
    pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);

    for (i = 0; i < N; i++) {
        ids[i] = i;
        if (pthread_create(&tid[i], &tattr, filosofo, &ids[i]) != 0) {
            fprintf(stderr, "Error al crear el filosofo %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }

    sem_destroy(&mutex);
    for (i = 0; i < N; i++) {
        sem_destroy(&s[i]);
    }
    pthread_attr_destroy(&tattr);

    printf("Todos los filosofos terminaron de cenar.\n");
    return 0;
}
