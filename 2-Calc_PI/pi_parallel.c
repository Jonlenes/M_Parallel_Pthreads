#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

long thread_count;
unsigned int n; 
long long unsigned int in;
pthread_mutex_t mutex;

void *monte_carlo_pi(void *void_arg) 
{
	long long unsigned int in_local = 0, i;
	double x, y, d;
    unsigned int n_local = n / thread_count;
    unsigned int seed = (long) void_arg; //recebendo o seed da thread
    
	for (i = 0; i < n_local; i++) {
		
		x = ((rand_r(&seed) % 1000000)/500000.0)-1;
		y = ((rand_r(&seed) % 1000000)/500000.0)-1;
		d = ((x * x) + (y * y));
		
		if (d <= 1) in_local += 1;
	}
	
	// protegendo in para que somente uma thread possa atualiza-lo por vez
	pthread_mutex_lock(&mutex);
	in += in_local;
	pthread_mutex_unlock(&mutex);
	
	return NULL;
}

int main(void) {
	
	long i;
	long unsigned int duracao;
	struct timeval start, end;
	long seed;

	srand(time(NULL));
	pthread_mutex_init(&mutex, NULL);
	in = 0;

	scanf("%ld %u", &thread_count, &n);
	pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
	

    gettimeofday(&start, NULL);
	
    for (i = 0; i < thread_count; ++i) {
        seed = time(NULL);
        pthread_create(&thread_handles[i], NULL, monte_carlo_pi, (void *) seed);
	}
     
    for (i = 0; i < thread_count; ++i)  
		pthread_join(thread_handles[i], NULL);
	
	gettimeofday(&end, NULL);

	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	printf("%lf\n%lu\n", (4.0 * in / (double)n), duracao);
	
	free(thread_handles);

	return 0;
}
