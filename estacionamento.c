#include "estacionamento.h"

pthread_mutex_t mutex;
sem_t vazio;
sem_t cheio;

int inserirPonteiro = 0, removerPonteiro = 0;

int entradaEstacionamento(carro item){
    sem_wait(&vazio);
    pthread_mutex_lock(&mutex);

    do{
        inserirPonteiro = rand() % Tamanho_Estacionamento;
    } while(estacionamento[inserirPonteiro].placaDoCarro[0] != '\0');

    estacionamento[inserirPonteiro] = item;
    printf("Carro %s estacionado na vaga %d.\n", item.placaDoCarro, (inserirPonteiro+1));

    int i;
    for(i = 0; i < Tamanho_Estacionamento; i++){
        if(estacionamento[i].placaDoCarro[0] == '\0'){
            break;
        }
        if(i == Tamanho_Estacionamento - 1){
            printf("O estacionamento está cheio.\n");

        }
    }
    pthread_mutex_unlock(&mutex);
    sem_post(&cheio);
    return 0;
}

int saidaEstacionamento(){
    sem_wait(&cheio);
    pthread_mutex_lock(&mutex);

    do{
        removerPonteiro = rand() % Tamanho_Estacionamento;
    } while(estacionamento[removerPonteiro].placaDoCarro[0] == '\0');

    struct timespec mostrarTempo;
    clock_gettime(CLOCK_REALTIME, &mostrarTempo);

    double tempoPassado = ((mostrarTempo.tv_sec - estacionamento[removerPonteiro].tempoDeChegada.tv_sec) 
    + (mostrarTempo.tv_nsec - estacionamento[removerPonteiro].tempoDeChegada.tv_nsec) / CLOCK_PRECISION);

    printf("Carro %s saiu da vaga %d e ficou %.2f segundos no estacionamento.\n", estacionamento[removerPonteiro].placaDoCarro, (removerPonteiro+1), tempoPassado);

    estacionamento[removerPonteiro] = estacionamentoVazio;

    int i;
    for(i = 0; i < Tamanho_Estacionamento; i ++){
        if(estacionamento[i].placaDoCarro[0] != '\0'){
            break;
        }
        if(i == Tamanho_Estacionamento - 1){
            printf("O estacionamento está vazio.\n");
        }
    }
    pthread_mutex_unlock(&mutex);
    sem_post(&vazio);
    return 0;
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, "Portuguese");
    int i = 0;
    for(i = 0; i < Tamanho_Estacionamento; i ++){
        estacionamento[i] = estacionamentoVazio;
    }

    printf("Bem vindo ao Estacionamento.\n");
    printf("Aperte m ou M para mostrar o estado atual do estacionamento.\n");
    printf("Aperte s ou S para encerrar a simulação do estacionamento.\n");
    printf("Aperte o enter para começar a simulação.\n");
    char opcaoDoUser;

    while(opcaoDoUser != '\n'){
        opcaoDoUser = getchar();
    }

    //Cria a Chegada, Saída e Monitores //

    pthread_mutex_init(&mutex, NULL);
    sem_init(&vazio, 0, Tamanho_Estacionamento);
    sem_init(&cheio, 0, 0);
    srand(time(0));

    pthread_t chegadaId;
    pthread_attr_t chegadaTr;
    pthread_attr_init(&chegadaTr);
    pthread_create(&chegadaId, &chegadaTr, chegada, NULL); //Producer Thread

    pthread_t saidaId;
    pthread_attr_t saidaTr;
    pthread_attr_init(&saidaTr);
    pthread_create(&saidaId, &saidaTr, saida, NULL); //Consumer Thread

    pthread_t monitorId;
    pthread_attr_t monitorTr;
    pthread_attr_init(&monitorTr);
    pthread_create(&monitorId, &monitorTr, monitor, NULL); //Monitor Thread

    pthread_join(monitorId, NULL);

    return 0;
}

//Opera em um ciclo sleep de 0.5s //

void *chegada(void *param){
    carro novoCarro;

    while(TRUE){
        struct timespec chegando; //Calcular o tempo gasto por um pedido
        clock_gettime(CLOCK_REALTIME, &chegando);

        int i = 0;
        for(i = 0; i < 3; i++){
            novoCarro.placaDoCarro[i] = (char)((rand() % 26) + 65);
        }                                                                      //Gera a placa do carro
        for(i = 3; i < 6; i++){
            novoCarro.placaDoCarro[i] = (char)((rand() % 10) + 48);
        }

        novoCarro.tempoDeChegada = chegando;
        if((rand() % 100) <= 60){ // Se o número aleatório abaixo entre 0-100 também estiver entre 0-60 - (60%), então se aloca um lugar no estacionamento //
            if(entradaEstacionamento(novoCarro)){
                fprintf(stderr, "Erro!");
            }
        }
        struct timespec tempo1, tempo2;
        tempo1.tv_sec = 0;
        tempo2.tv_nsec = 500000000L;

        nanosleep(&tempo1, &tempo2);
    }
}

//Opera em um ciclo sleep de 0.5s //

void *saida(void *param){
    while(TRUE){
        if((rand() % 100) <= 40){ //Se o número aleatório abaixo entre 0-100 também estiver entre 0-40 - (40%), então se retira o carro do estacionamento //
            if(saidaEstacionamento()){
                fprintf(stderr, "Erro!");
            }
        }
        struct timespec tempo1, tempo2;
        tempo1.tv_sec = 0;
        tempo2.tv_nsec = 500000000L;

         nanosleep(&tempo1, &tempo2);
    }
}

void *monitor(void *param){ //Sair e/ou entrar na simulação //
    int sairDoPrograma = 0;
    char entrada;

    while(sairDoPrograma != TRUE){
        scanf("%c", &entrada);
        if(entrada == 'm' || entrada == 'M'){
            mostrarEstacionamento();
        }else if(entrada == 's' || entrada == 'S'){
            sairDoPrograma = TRUE;
            printf("A simulação acabou!\n");
        }
    }
}

void mostrarEstacionamento(){ //Mostra o estado do Estacionamento // 
    int i;
    printf("Estado do Estacionamento:\n");

    struct timespec mostrarTempo;
    clock_gettime(CLOCK_REALTIME, &mostrarTempo);
    double tempoPassado;

    for(i = 0; i < Tamanho_Estacionamento; i++){
        if(estacionamento[i].placaDoCarro[0] == '\0'){
            printf("%d: Vazio\n", (i+1));
        }else{
            tempoPassado = ((mostrarTempo.tv_sec - estacionamento[i].tempoDeChegada.tv_sec) 
            + (mostrarTempo.tv_nsec - estacionamento[i].tempoDeChegada.tv_nsec) / CLOCK_PRECISION);
            printf("%d: %s ( está estacionado por %.2f segundos)\n", (i+1), estacionamento[i].placaDoCarro, tempoPassado);
        }
    }
}