#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
  unsigned int *bloco;
  unsigned int intervalo_bloco;
  unsigned int num_baldes;
} bloco_t;

typedef struct {
  unsigned int *vetor;
  unsigned int tamanho_vetor;
  unsigned int num_blocos;
  unsigned int indice_bloco;
} vetor_t;

typedef struct {
  unsigned int *balde;
  unsigned int *tamanho;
} balde_t;

// Funcao de ordenacao fornecida. Não pode alterar.
void bubble_sort(int *v, int tam){
    int i, j, temp, trocou;

    for(j = 0; j < tam - 1; j++){
        trocou = 0;
        for(i = 0; i < tam - 1; i++){
            if(v[i + 1] < v[i]){
                temp = v[i];
                v[i] = v[i + 1];
                v[i + 1] = temp;
                trocou = 1;
            }
        }
        if(!trocou) break;
    }
}

// Funcao para imprimir um vetor. Não pode alterar.
void imprime_vet(unsigned int *v, int tam) {
    int i;
    for(i = 0; i < tam; i++)
        printf("%d ", v[i]);
    printf("\n");
}

// Funcao para ler os dados de um arquivo e armazenar em um vetor em memoroa. Não pode alterar.
int le_vet(char *nome_arquivo, unsigned int *v, int tam) {
    FILE *arquivo;

    // Abre o arquivo
    arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    // Lê os números
    for (int i = 0; i < tam; i++)
        fscanf(arquivo, "%u", &v[i]);

    fclose(arquivo);

    return 1;
}

void* particionar_vetor(void* args){
    vetor_t vetor = *((vetor_t*) args);
    bloco_t* bloco = malloc(sizeof(bloco_t)); 
    unsigned int tamanho_do_intervalo = vetor.tamanho_vetor / vetor.num_blocos;
    unsigned int resto = vetor.tamanho_vetor % vetor.num_blocos;
    unsigned int *endereco_bloco = malloc(tamanho_do_intervalo*sizeof(unsigned int));
    
    bloco->bloco = endereco_bloco;
    bloco->intervalo_bloco = tamanho_do_intervalo;
    bloco->num_baldes = vetor.num_blocos;
    
    for (unsigned int i = 0; i < tamanho_do_intervalo; i++) {
      endereco_bloco[i] = vetor->vetor[vetor->indice_bloco*tamanho_do_intervalo + i];
    }
    free(&vetor);
    return (void*) bloco;
}

void imprimir_matriz(unsigned int **matriz, unsigned int num_linhas, unsigned int num_colunas){
    for (int i = 0; i < num_linhas; i++){
        for (int j = 0; j < num_colunas; j++){
            printf("%d ", particao[i][j]);
          }
            printf("\n");
      }
}

void* decompor_bloco(void* args){
    bloco_t var = ((bloco_t) args);
    unsigned int intervalo_balde = var.intervalo_bloco / var.num_baldes;

    if (intervalo_balde == 0 ) intervalo_balde = 1;

    unsigned int *valor_de_retorno = malloc(var.num_blocos * sizeof(unsigned int));

    return_baldes_t * ret = malloc(sizeof(return_baldes_t));
    ret->tamanho = arg num_baldes;
    ret->baldes =

    for (int i = 0; i < var.num_baldes; i++) {
        unsigned int* balde = malloc(tam_max_bloco * sizeof(unsigned int*));
        valor_de_retorno[i] = balde;
    }
    // parei aq
    for (unsigned int i = 0; i < ; i++) {

    }
    return (void*) valor_de_retorno;
  }

// Funcao principal de ordenacao. Deve ser implementada com base nas informacoes fornecidas no enunciado do trabalho.
// Os numeros ordenados deverao ser armazenanos no proprio "vetor".
int sort_paralelo(unsigned int *vetor, unsigned int tam, unsigned int ntasks, unsigned int nthreads) {
    // NOTE: é mais eficiente criar uma cópia do vetor e usar ela na ordenação

    if (ntasks < nthreads) nthreads = ntasks;

    pthread_t threads[nthreads];
    return_baldes_t results[nthreads];
    unsigned int intervalo = tam/ntasks;
    unsigned int pos = 0;
    unsigned int resto = tam % ntasks;

// Fase 1: Particionar o vetor
// Fase 2: Dividir blocos da partição em baldes
// Fase 3: Aplicar bubble sort em cada balde
// Fase 4: Mesclar conteúdo dos baldes e armazenar em contêineres
// Fase 5: Aplicar bubble sort em cada contêiner
// Fase 6: Concatenar contêineres

    // Fase 1
    unsigned int *particao[ntasks]

    for (unsigned int i = 0; i < ntasks; i++) {
      vetor_t* subvetor = malloc(sizeof(vetor_t));
      subvetor->vetor = vetor;
      subvetor->tamanho_vetor = tam;
      subvetor->num_blocos = ntasks;
      subvetor->indice_bloco = i;
      pthread_create(&threads[i], NULL, particionar_vetor, (void *) subvetor);
    }

    for (unsigned int i = 0; i < ntasks, i++) {
      pthread_join(threads[i], (void *)particao[i]);
    }

    // DEBUG
    imprimir_matriz(particao, ntasks, intervalo);

    // Fase 2
    balde_t* barril[ntasks]; // barril := lista com todos os baldes

    for (unsigned int i = 0; i < ntasks; i++) {
        bloco_t * args = malloc(sizeof(bloco_t));
        args->bloco = particao[i];
        args->intervalo_bloco = tam_bloco + (i < resto ? 1 : 0);
        args->num_baldes = ntasks;
        args->tam_max_bloco = tam;
        pthread_create(&threads[i], NULL, decompor_bloco, (void*)args);
        pos_atual += args->intervalo_bloco;
    }

    for (unsigned int i = 0; i < ntasks; i++) {
        pthread_join(threads[i], (void *) barril[i]);
    }
}

// Funcao principal do programa. Não pode alterar.
int main(int argc, char **argv) {
    
    // Verifica argumentos de entrada
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <input> <nnumbers> <ntasks> <nthreads>\n", argv[0]);
        return 1;
    }

    // Argumentos de entrada
    unsigned int nnumbers = atoi(argv[2]);
    unsigned int ntasks = atoi(argv[3]);
    unsigned int nthreads = atoi(argv[4]);
    
    // Aloca vetor
    unsigned int *vetor = malloc(nnumbers * sizeof(unsigned int));

    // Variaveis de controle de tempo de ordenacao
    struct timeval inicio, fim;

    // Le os numeros do arquivo de entrada
    if (le_vet(argv[1], vetor, nnumbers) == 0)
        return 1;

    // Imprime vetor desordenado
    imprime_vet(vetor, nnumbers);

    // Ordena os numeros considerando ntasks e nthreads
    gettimeofday(&inicio, NULL);
    sort_paralelo(vetor, nnumbers, ntasks, nthreads);
    gettimeofday(&fim, NULL);

    // Imprime vetor ordenado
    imprime_vet(vetor, nnumbers);

    // Desaloca vetor
    free(vetor);

    // Imprime o tempo de ordenacao
    printf("Tempo: %.6f segundos\n", fim.tv_sec - inicio.tv_sec + (fim.tv_usec - inicio.tv_usec) / 1e6);
    
    return 0;
}
