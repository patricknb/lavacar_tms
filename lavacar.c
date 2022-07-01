# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <semaphore.h>

# define num_clientes 15
# define num_funcionarios 1
# define num_vagas 3


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_atendimento = PTHREAD_MUTEX_INITIALIZER;


sem_t disponivel;
sem_t atendimento;
sem_t vagas;
int vagas_ocupadas = 0;
int fim;

int desiste(int id){
	printf("o carro %ld nao conseguiu entrar, pois nao havia vaga disponivel!\n", id);
	pthread_exit(NULL);
}

int sair(){
	pthread_exit(NULL);
}


void* funcionario(void *arg){
	int id = *(int *)arg;
	while(1){
		sem_wait(&atendimento);
		pthread_mutex_lock(&mutex_atendimento);
		
		printf("funcionario %ld esta chamando o proximo carro.\n", id);
		
		pthread_mutex_unlock(&mutex_atendimento);
		sem_post(&disponivel);
		
		if (fim == 1){
			sair();
		}
		
	}
}

void* cliente(void *arg){
	int id = *(int *)arg;
	sleep(1);
	while(1){
		if (vagas_ocupadas == num_vagas){
			desiste(id);
		
		} else {
			sem_wait(&vagas);
			

			pthread_mutex_lock(&mutex);
			vagas_ocupadas += 1;
			printf("o carro %ld entrou na fila do lava carro ", id);
			printf("(vagas ocupadas:%ld)\n", vagas_ocupadas);
			pthread_mutex_unlock(&mutex);
		
			
			sem_wait(&disponivel);
			pthread_mutex_lock(&mutex_atendimento);
			
			printf("carro %ld está sendo lavado.\n", id);
			sleep(3);
			printf("carro %ld foi lavado.\n", id);
			
			pthread_mutex_unlock(&mutex_atendimento);
			sem_post(&atendimento);
			
			pthread_mutex_lock(&mutex);
			vagas_ocupadas -= 1;
			printf("o carro %ld saiu da fila ", id);
			printf("(vagas ocupadas = %ld)\n", vagas_ocupadas);
			pthread_mutex_unlock(&mutex);
			
			sem_post(&vagas);
			
			
			
			
			
			sair();
			
		}
	}
}


int main(){
	
	pthread_t tfuncionario[num_funcionarios]; int f_ids[num_funcionarios];
	pthread_t tcliente[num_clientes]; int c_ids[num_clientes];
	
	
	sem_init(&vagas, 0, num_vagas);
	sem_init(&disponivel, 0, 1);
	sem_init(&atendimento, 0, 1);
	
	printf("lava car aberto!\n");
	
	
	for (int i = 0; i < num_funcionarios; i++){
		f_ids[i] = i;
		pthread_create(&tfuncionario[i], NULL, funcionario, &f_ids[i]);
		
	}

	for (int i = 0; i < num_clientes; i++){
		c_ids[i] = i;
		pthread_create(&tcliente[i], NULL, cliente, &c_ids[i]);
		sleep(2);
		
	}
	
	for (int i  = 0; i < num_clientes; i++){
		pthread_join(tcliente[i], NULL);
		sem_destroy(&vagas);
		sem_destroy(&disponivel);
		sem_destroy(&atendimento);
		fim = 1;
		
	}
	
	for (int i  = 0; i < num_clientes; i++){
		pthread_join(tfuncionario[i], NULL);
	}

	return 0;
}
