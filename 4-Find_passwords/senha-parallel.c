#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

int thread_count;
const int password_begin = 10000; //Menor senha considerada - conforme enunciado
const int password_end = 500000; //Maior senha considerada - conforme enunciado
int block_size; //Tamanho do bloco processado por thread
const char finalcmd[300] = "unzip -P%d -t %s 2>&1";
char filename[100];
sem_t semaphore;

FILE* popen(const char* command, const char* type);
void *findPassword(void *arg);

double rtclock()
{
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday(&Tp, &Tzp);
    if (stat != 0)
        printf("Error return from gettimeofday: %d", stat);
    return (Tp.tv_sec + Tp.tv_usec * 1.0e-6);
}

void *threadController(void *arg)
{
    block_size = (password_end - password_begin) / thread_count;

    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
    sem_init(&semaphore, 0, 1);

    for (long i = 0; i < thread_count; ++i)
        pthread_create(&thread_handles[i], NULL, findPassword, (void *) i);

    for (long i = 0; i < thread_count; ++i)
        pthread_join(thread_handles[i], NULL);

    free(thread_handles);
    sem_destroy(&semaphore);

    return arg;
}

void *findPassword(void *arg)
{
    FILE* fp;
    char ret[200];
    char cmd[400];

    long my_rank = (long) arg;
    int my_first_i = password_begin + my_rank * block_size;
    int my_last_i = my_first_i + block_size;
    int password_found = 0;
    int value;

    for (int i = my_first_i; i <= my_last_i; ++i) {
        sprintf((char*)&cmd, finalcmd, i, filename);

        sem_getvalue(&semaphore, &value);
        if (!value) return 0; //Se outra thread já achou a senha, então esta pode ser finalizada

        fp = popen(cmd, "r");
        while (!feof(fp)) {
            fgets((char*)&ret, 200, fp);
            if (strcasestr(ret, "ok") != NULL) {
                printf("Senha:%d\n", i);
                sem_wait(&semaphore); //Decrementando o contador do semaforo
                password_found = 1; //Sinalizando que a senha foi encontrada
                break; //Parando o while pois a senha foi encontrada
            }
        }
        pclose(fp);
        if (password_found) return 0; //Se a password foi encontrada, a função pode ser finalizada
    }
    return 0;
}

int main()
{
    double t_start, t_end;

    scanf("%d", &thread_count);
    scanf("%s", filename);

    pthread_t thr_hand_controller;

    t_start = rtclock();

    //Cria e espera a thread que controla todo o processamento
    pthread_create(&thr_hand_controller, NULL, threadController, NULL);
    pthread_join(thr_hand_controller, NULL);

    t_end = rtclock();

    fprintf(stdout, "%0.6lf\n", t_end - t_start);
}
