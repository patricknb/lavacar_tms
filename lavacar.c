# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <semaphore.h>

# define num_clientes 10
# define num_funcionarios 1
# define num_vagas 3


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

sem_t disponivel;
sem_t ocupado;
sem_t vagas;
int vagas_ocupadas = 0;


int sair(){
	pthread_exit(NULL);
}


void* funcionario_lavacar(void *arg){
	int id = *(int *)arg;
		
	for(int i; i < num_clientes; i++){
		// seção critica
		sem_wait(&disponivel);
		pthread_mutex_lock(&mutex);
	
		printf("funcionario %ld esta disponivel para atendimentos.\n", id);
		
		pthread_mutex_unlock(&mutex);
		sem_post(&ocupado);
		//seção critica
	}
	printf("turno do funcionario acabou.\n");
	sair();
	
}

void* cliente_lavacar(void *arg){
	int id = *(int *)arg;
	
	
	pthread_mutex_lock(&mutex2);
	
	vagas_ocupadas += 1;
	
	if (vagas_ocupadas > num_vagas){
		vagas_ocupadas -= 1;
		printf("cliente %ld desistiu, pois nao ha vagas.\n", id);
		pthread_mutex_unlock(&mutex2);
		sair();
	} else {
		printf("cliente %ld entrou em uma vaga.\n", id);
	}
	
	pthread_mutex_unlock(&mutex2);
	
	sem_wait(&vagas);
	
	
	//seção critica
	sem_wait(&ocupado);
	pthread_mutex_lock(&mutex);
	
	printf("cliente %ld vai ser atendido.\n", id);
	sleep(2);
	
	pthread_mutex_unlock(&mutex);
	sem_post(&disponivel);
	// seção critica
	
	sem_post(&vagas);
	
	pthread_mutex_lock(&mutex2);
	
	vagas_ocupadas -= 1;
	printf("cliente %ld foi atendido e esta saindo.\n", id);
	
	pthread_mutex_unlock(&mutex2);
	
	
	sair();
}


int main(){
	pthread_t tcliente[num_clientes]; int c_ids[num_clientes];
	sem_init(&vagas, 0, num_vagas);
	sem_init(&disponivel, 0, 1);
	sem_init(&ocupado, 0, 1);
	
	
	pthread_t tfuncionario; int f_id = 0;
	pthread_create(&tfuncionario, NULL, funcionario_lavacar, &f_id);
	
	for (int i = 0; i < num_clientes; i++){
		c_ids[i] = i;
		pthread_create(&tcliente[i], NULL, cliente_lavacar, &c_ids[i]);
		sleep(2);
	}
	
	for (int i  = 0; i < num_clientes; i++){
		pthread_join(tcliente[i], NULL);
		sem_destroy(&vagas);
		sem_destroy(&disponivel);
		sem_destroy(&ocupado);
	}
	
	pthread_join(tfuncionario, NULL);
	return 0;
}
