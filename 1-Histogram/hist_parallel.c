#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

/* funcao que calcula o minimo valor em um vetor */
double min_val(double * vet,int nval) {
	int i;
	double min;

	min = FLT_MAX;

	for(i=0;i<nval;i++) {
		if(vet[i] < min)
			min =  vet[i];
	}
	
	return min;
}

/* funcao que calcula o maximo valor em um vetor */
double max_val(double * vet, int nval) {
	int i;
	double max;

	max = FLT_MIN;

	for(i=0;i<nval;i++) {
		if(vet[i] > max)
			max =  vet[i];
	}
	
	return max;
}

double h, *values, max, min;
int n, count_val, i, *vet, threads_count;
	
	
/* conta quantos valores no vetor estao entre o minimo e o maximo passados como parametros */
void *calcHistogram(void *void_arg) {
	
	long my_rank = (long) void_arg;
	int i, j, count;
	double min_t, max_t;

	//Calculado os limites da interação da thread atual	
	int nbins_local = n / threads_count;
	int start = my_rank * nbins_local;
	int end = start + nbins_local; 

	//Resolvendo o problema da divisão inexata
	if (my_rank == threads_count - 1) end = n - 1;

	//Calculado o histograma de [start-end]
	for(j = start; j <= end; ++j) {
		count = 0;
		min_t = min + j * h;
		max_t = min + (j + 1) * h;
		for(i = 0; i < count_val; ++i) {
			if((values[i] <= max_t && values[i] > min_t) || (j == 0 && values[i] <= min_t) )
				count++;
		}

		vet[j] = count;
	}

	return NULL;
}

int main(int argc, char * argv[]) {
	
	long unsigned int duracao;
	struct timeval start, end;
	pthread_t *thread_handles;

	//threadsCount	-	numero de threads
	//countVal		- 	entrada do numero de dados
	//n				-	numero de barras do histograma a serem calculadas
	scanf("%d %d %d", &threads_count, &count_val, &n);
	threads_count = 8;

	/* vetor com os dados */
	values = (double *)malloc(count_val * sizeof(double));
	vet = (int *)malloc(n * sizeof(int));
	thread_handles = malloc(threads_count * sizeof(pthread_t));

	/* entrada dos dados */
	for(i = 0; i < count_val; i++)
		scanf("%lf", &values[i]);

	/* calcula o minimo e o maximo valores inteiros */
	min = floor(min_val(values, count_val));
	max = ceil(max_val(values, count_val));

	/* calcula o tamanho de cada barra */
	h = (max - min)/n;

	gettimeofday(&start, NULL);
	
	/* chama a funcao */
	for (i = 0; i < threads_count; ++i)
		pthread_create(&thread_handles[i], NULL, calcHistogram, (void *) (long)i);
	
	for (i = 0; i < threads_count; ++i)
		pthread_join(thread_handles[i], NULL);

	gettimeofday(&end, NULL);

	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - \
			  (start.tv_sec * 1000000 + start.tv_usec));

	printf("%.2lf",min);	
	for(i=1;i<=n;i++) {
		printf(" %.2lf",min + h*i);
	}
	printf("\n");

	/* imprime o histograma calculado */	
	printf("%d",vet[0]);
	for(i=1;i<n;i++) {
		printf(" %d",vet[i]);
	}
	printf("\n");
	
	
	/* imprime o tempo de duracao do calculo */
	printf("%lu\n",duracao);

	free(vet);
	free(values);

	return 0;
}
/***********************************************************************
 * 						TRABALHO COMPLEMENTAR
 * 
 * 			Threads		1	 2	 	4	 	8	 	16
 * -----------------------------------------------------
 * arq1.in 	Speedup 	1	1.29	1.55	1.97	2.24	
 *			Eficiência	1	0.65	0.39	0.25	0.14
 * 	 
 * arq2.in	Speedup		1	1.90	2.80	3.17	3.76			
 * 			Eficiência	1	0.95	0.70	0.40	0.23
 * 
 * arq3.in  Speedup		1	1.92	2.85	3.18	3.4	
 * 			Eficiência	1	0.96	0.71	0.40	0.21	
 * 
 * - Os speedups cresceram conforme aumentada a quantidade de threads
 * - Por outro lado, a eficiência foi só diminuido
 * 
 * O percentual do programa que é paralelizavél é aproximandamente: 
 * 	quando mais a entrada aumenta, mais aumenta o percentual paralelizavel,
 *  no entanto, isso nao passa dos 90%, aproximadamente. 
 * 
 * arq1.in
 * 		T(par) = 1917
 * 		T(total) = 3943
 * 		T(par) / T(total) = 0,48
 * 
 * arq2.in
 * 		T(par) = 55072
 * 		T(total) = 65044
 * 		T(par) / T(total) = 0,84
 * 
 * arq3.in
 * 		T(par) = 430625
 * 		T(total) = 474400
 * 		T(par) / T(total) = 0,90	 
 * *********************************************************************/
