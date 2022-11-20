#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>

#define Tamanho_Estacionamento 100
#define TRUE 1
#define CLOCK_PRECISION 1E9  /* 1 bilhão */

struct estacionamento{
	char placaDoCarro[7];
	struct timespec tempoDeChegada;
};

typedef struct estacionamento carro;

carro estacionamento[Tamanho_Estacionamento];

const carro estacionamentoVazio = { "\0", { -1, 0 } };

int entradaEstacionamento(carro item);

int saidaEstacionamento();

void *chegada(void *param);

void *saida(void *param);

void *monitor(void *param);

void mostrarEstacionamento();

