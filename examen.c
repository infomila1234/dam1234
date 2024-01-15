#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio_ext.h>

#include <pthread.h>
#include <semaphore.h>


#define M 		10
#define S		5
#define T		10

#define ARXIU_ENCERTS	"encerts.txt"
#define ARXIU_ERRORS	"errors.txt"


typedef struct {
	int nPregReal;
	int nEncerts;
	int nErrors;
	int nTempsExcessiu;
} t_data;

sem_t s_in, s_outCor, s_outIncor; 
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

int generaNum() {
	// Generació del número aleatori:
	srand(time(NULL));
	int numAleatori = rand() % M + 1;
	return numAleatori;
}


void *pregunta(void *data) {
	bool final = false;
	bool correcte = false;
	int p1, p2, resC, resUsu;
	
	while (!final) {
		sem_wait(&s_in);
		pthread_mutex_lock(&mut);
	
		p1 = generaNum();
		sleep(1);
		p2 = generaNum();
		resC = p1 * p2;
		
		// Preguntar el resultat a l'usuari:
		printf("Els productes de la multiplicació són %i i %i. Respon: ",p1 ,p2);
		scanf("%i",&resUsu);
		__fpurge(stdin);
		
		if(resUsu == resC){
			correcte = true;
		} else if (resUsu == 0) {
			final = true;
		} else {
			correcte = false;
		}
		
		pthread_mutex_unlock(&mut);	
		
		
			// Llógica per determinar a quin fil dirigir-se
		// L'usuari ha respós correctament:
		if(correcte && !final) {
			printf("Resposta correcta.\n");
			sem_post(&s_outCor);
		} 
		// L'usuari no ha respós correctament:
		else if (!correcte && !final) {
			printf("Resposta incorrecta.\n");
			sem_post(&s_outIncor);
		}
		
		if(!final) {
		
		}
		
	}
	
	
}

void *correcte(void *data) {
	bool final = false;

	while(!final) {
		sem_wait(&s_outCor);
		pthread_mutex_lock(&mut);
		// Llógica de la funció:
		
		
		
		pthread_mutex_unlock(&mut);
		
		if(!final) {
			sleep(2);
			printf("Desant a l'arxiu...\n");
		}
		sem_post(&s_in);
	}


}

void *incorrecte(void *data) {
	bool final = false;

	while(!final) {
		sem_wait(&s_outIncor);
		pthread_mutex_lock(&mut);
		// Llógica de la funció:
		
		
		
		
		
		pthread_mutex_unlock(&mut);
		
		if(!final) {
			sleep(2);
			printf("Desant a l'arxiu...\n");
		}
		sem_post(&s_in);
	}


}

int main(int argc, char *argv[]) {
	pthread_t f_pregunta;
	pthread_t f_resCor;
	pthread_t f_resIncor;
	t_data data;
	
	// Inicialitzo les dades del struct:
	data.nPregReal = 0;
	data.nEncerts = data.nErrors = 0;
	data.nTempsExcessiu = 0;
	
	// Inicialitzo els semàfors:
	sem_init (&s_in, 0, 1);
  	sem_init (&s_outCor, 0, 0);
  	sem_init (&s_outIncor, 0, 0);
  	
  	// Activo els threads:
  	pthread_create(&f_pregunta, NULL, pregunta, (void *)&data);
  	pthread_create(&f_resCor, NULL, correcte, (void *)&data);
  	pthread_create(&f_resIncor, NULL, incorrecte, (void *)&data);
  	
	
	printf("Introdueix un 0 en qualsevol resposta per aturar el programa... \n\n");
	
	
	// Espero a que els threads acabin:
	pthread_join(f_pregunta, NULL);
	pthread_join(f_resCor, NULL);
	pthread_join(f_resIncor, NULL);
	
	
	// Destrueixo els semàfors:
	sem_destroy (&s_in);
   	sem_destroy (&s_outCor);
   	sem_destroy (&s_outIncor);
	
	return 1;
}