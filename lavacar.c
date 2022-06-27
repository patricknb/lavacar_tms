# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <semaphore.h>

# define num_clientes 10
# define num_funcionarios 1
# define num_vagas 3

pthread_mutex_t funcionario = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t vagas;

int vagas_ocupadas = 0;

void* cliente_lavando_carro(void *arg){
	int id = *(int *)arg;
	
	if (vagas_ocupadas > 2){
		printf("o carro %ld tentou entrar na vaga, mas estava cheio...\n");
		pthread_exit(NULL);
	}
	
	
	sem_wait(&vagas);
	if (vagas_ocupadas < num_vagas){
		pthread_mutex_lock(&mutex);
		vagas_ocupadas += 1;
		printf("o carro %ld está esperando na vaga  ", id);
		printf("(vagas = %ld)\n", vagas_ocupadas);
		pthread_mutex_unlock(&mutex);
		
	}
	
	pthread_mutex_lock(&funcionario);
	// sec crit
	printf("carro %ld está sendo atendido pelo funcionario\n", id);
	sleep(4);
	printf("carro %ld foi atendido e está de saida...\n", id);
	// sec crit
	pthread_mutex_unlock(&funcionario);
	
	if (vagas_ocupadas > 0){
		pthread_mutex_lock(&mutex);
		vagas_ocupadas -= 1;
		printf("o carro %ld saindo da vaga  ", id);
		printf("(vagas = %ld)\n", vagas_ocupadas);
		pthread_mutex_unlock(&mutex);
	
	sem_post(&vagas);
	
	pthread_exit(NULL);
	}
}



int main(){
	pthread_t tcliente[num_clientes]; int c_ids[num_clientes];
	sem_init(&vagas, 0, num_vagas);
	
	for (int i = 0; i < num_clientes; i++){
		c_ids[i] = i;
		pthread_create(&tcliente[i], NULL, cliente_lavando_carro, &c_ids[i]);
		sleep(2);
	}
	
	for (int i  = 0; i < num_clientes; i++){
	
	pthread_join(tcliente[i], NULL);
	sem_destroy(&vagas);
		
	}
	
	return 0;
}
