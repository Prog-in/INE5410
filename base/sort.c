#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    unsigned int *vetor;
    unsigned int tam_vetor;
    unsigned int inicio_bloco;
    unsigned int intervalo;
} pedasco_t;

typedef struct {
    unsigned int *ret_vec;
    unsigned int ret_tam;
} ret_t;

typedef struct {
  unsigned int *bloco;
  unsigned int intervalo_bloco;
  unsigned int num_baldes;
  unsigned int upper_balde;
} bloco_t;

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

void* tramonteiro(void *tramontado_void) {
    pedasco_t *tramontado = (pedasco_t*) tramontado_void;
    unsigned int* ret_vec = malloc(tramontado->tam_vetor * sizeof(unsigned int));
    unsigned int index = 0;
    for (int i = 0; i < tramontado->tam_vetor; i++) {
        if (tramontado->vetor[i] >= tramontado->inicio_bloco && tramontado->vetor[i] <= tramontado->inicio_bloco+tramontado->intervalo) {
            ret_vec[index++] = tramontado->vetor[i];
        }
    }
    ret_t *ret = malloc(sizeof(ret_t));
    ret->ret_vec = ret_vec;
    ret->ret_tam = index+1;
    pthread_exit((void*) ret);
}

unsigned int** particionar_vetor(unsigned int *vetor, unsigned int tamanho_vetor, unsigned int num_blocos){
  unsigned int **particao = malloc(num_blocos*sizeof(unsigned int*));
  unsigned int tamanho_do_intervalo = tamanho_vetor / num_blocos;
  unsigned int resto = tamanho_vetor % num_blocos;
  for (unsigned int i = 0; i < num_blocos; i++){
    unsigned int *bloco = malloc(tamanho_do_intervalo*sizeof(unsigned int));
    particao[i] = bloco;
    for(unsigned int j = 0; j < tamanho_do_intervalo; j++) {
      bloco[j] = vetor[i*tamanho_do_intervalo + j];
    }
  }
  return particao;
}

void imprimir_matriz(unsigned int **matriz, unsigned int num_linhas, unsigned int num_colunas){
    for (int i=0; i < num_linhas; i++){
      for (int j=0; j < num_colunas; j++){
        printf("%d ", particao[i][j]);
      }
      printf("\n");
    }
}

void* decompor_bloco(void* info_bloco){
  bloco_t var = *((bloco_t*) info_bloco);
  unsigned int intervalo_balde = var.intervalo_bloco / var.num_baldes;
  unsigned int **valor_de_retorno = malloc(var.num_blocos * sizeof(unsigned int*));
  for (int i = 0; i < var.num_blocos; i++) {
    unsigned int* balde = malloc(upper_balde * sizeof(unsigned int*));
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
    unsigned int intervalo = tam/ntasks;
    pthread_t threads[nthreads];
    pedasco_t* pedascos[nthreads];
    unsigned int resto = tam % ntasks;
    //TODO(Hélcio): paralelizar iso
    //TODO(Hélcio): tratar RESTO
    unsigned int **particao = particionar_vetor(vetor, tam, ntasks);
    imprimir_matriz(particao, ntasks, intervalo);
    for (int i = 0; i < ntasks, i++){
      unsigned int *bloco = particao[i];
      pthread_create(threads+i, NULL, decompor_bloco, (void*)bloco);
    }
    unsigned int **baldes = malloc(ntasks*sizeof(unsigned int**));
    for (int i = 0; i < ntasks, i++){
      pthread_join(threads[i], baldes[i]);
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
