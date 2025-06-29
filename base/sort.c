#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    unsigned int *vetor;
    unsigned int tamanho_vetor;
    unsigned int num_blocos;
    unsigned int indice_bloco;
    unsigned int resto;
} vetor_t;

typedef struct {
    unsigned int *bloco;
    unsigned int tamanho_bloco;
    unsigned int num_baldes;
    unsigned int max_int;
    unsigned int resto;
} bloco_t;

typedef struct {
    unsigned int *itens;
    unsigned int tamanho;
} arr_t;

typedef arr_t balde_t;

typedef struct {
    balde_t **baldes;
    unsigned int num_baldes;
} barril_t;

typedef struct {
    barril_t **barris;
    unsigned int indice_balde;
    unsigned int num_barris;
    unsigned int tam_vetor;
} caixote_t;

typedef arr_t conteiner_t;

typedef struct {
    unsigned int *vetor;
    conteiner_t *container;
    unsigned int inicio;
} vetorizado_t;

// Funcao de ordenacao fornecida. Não pode alterar.
void bubble_sort(int *v, int tam) {
    int i, j, temp, trocou;

    for (j = 0; j < tam - 1; j++) {
        trocou = 0;
        for (i = 0; i < tam - 1; i++) {
            if (v[i + 1] < v[i]) {
                temp = v[i];
                v[i] = v[i + 1];
                v[i + 1] = temp;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
}

// Funcao para imprimir um vetor. Não pode alterar.
void imprime_vet(unsigned int *v, int tam) {
    int i;
    for (i = 0; i < tam; i++)
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

void *particionar_vetor(void *args) {
    vetor_t *vetor = (vetor_t *) args;
    bloco_t *bloco = malloc(sizeof(bloco_t));

    unsigned int resto = vetor->tamanho_vetor % vetor->num_blocos;
    unsigned int tamanho_adicional = vetor->indice_bloco < resto;
    unsigned int flag_offset_parcial = 0 < vetor->indice_bloco && vetor->indice_bloco <= resto;
    unsigned int flag_offset_total = vetor->indice_bloco > resto;
    unsigned int tamanho_do_intervalo = vetor->tamanho_vetor / vetor->num_blocos;

    bloco->tamanho_bloco = tamanho_do_intervalo + tamanho_adicional;
    bloco->bloco = malloc(bloco->tamanho_bloco * sizeof(unsigned int));
    bloco->num_baldes = vetor->num_blocos;
    bloco->max_int = vetor->tamanho_vetor - 1;
    bloco->resto = vetor->resto;

    for (unsigned int i = 0; i < bloco->tamanho_bloco; i++) {
        unsigned int start_index = vetor->indice_bloco * (tamanho_do_intervalo + flag_offset_parcial) + resto *
                                   flag_offset_total;
        bloco->bloco[i] = vetor->vetor[start_index + i];
    }
    return (void *) bloco;
}

void *decompor_bloco(void *args) {
    bloco_t *bloco = (bloco_t *) args;
    balde_t * *itens = malloc(sizeof(balde_t *) * bloco->num_baldes);

    barril_t *barril = malloc(sizeof(barril_t));
    barril->num_baldes = bloco->num_baldes;
    barril->baldes = itens;

    for (int i = 0; i < bloco->num_baldes; i++) {
        balde_t *balde = malloc(sizeof(balde_t));
        balde->itens = malloc(bloco->tamanho_bloco * sizeof(unsigned int));
        balde->tamanho = 0;
        itens[i] = balde;
    }

    for (int i = 0; i < bloco->tamanho_bloco; i++) {
        unsigned int inicio_balde = 0;
        // achar o balde para onde o elemento vai
        for (unsigned int j = 0; j < bloco->num_baldes; j++) {
            unsigned int intervalo_balde = (bloco->max_int + 1) / bloco->num_baldes;
            if (j < bloco->resto) intervalo_balde++;
            if (inicio_balde <= bloco->bloco[i] && bloco->bloco[i] <= inicio_balde + intervalo_balde - 1) {
                // achou
                itens[j]->itens[itens[j]->tamanho++] = bloco->bloco[i];
                break;
            }
            inicio_balde += intervalo_balde;
        }
    }

    free(bloco->bloco);
    free(bloco);

    return (void *) barril;
}

void *ordenar_baldes(void *arg) {
    barril_t *barril = (barril_t *) arg;

    for (int i = 0; i < barril->num_baldes; i++) {
        bubble_sort((int *) barril->baldes[i]->itens, (int) barril->baldes[i]->tamanho);
    }

    return NULL;
}

void *ordenar_container(void *arg) {
    conteiner_t *container = (conteiner_t *) arg;
    bubble_sort((int *) container->itens, (int) container->tamanho);

    return NULL;
}

void *conteinerizar(void *arg) {
    caixote_t *caixote = (caixote_t *) arg;
    conteiner_t *conteiner = malloc(sizeof(conteiner_t));
    conteiner->itens = malloc(caixote->tam_vetor * sizeof(unsigned int));
    conteiner->tamanho = 0;
    for (int i = 0; i < caixote->num_barris; i++) {
        balde_t *balde = (caixote->barris[i])->baldes[caixote->indice_balde];
        for (int j = 0; j < balde->tamanho; j++) {
            conteiner->itens[conteiner->tamanho++] = balde->itens[j];
        }
    }
    return (void *) conteiner;
}

void *vetorizador(void *arg) {
    vetorizado_t *vetorizado = (vetorizado_t *) arg;
    for (unsigned int i = 0; i < vetorizado->container->tamanho; i++) {
        vetorizado->vetor[vetorizado->inicio + i] = vetorizado->container->itens[i];
    }
    return NULL;
}

void imprimir_blocos(unsigned int quantidade, bloco_t **blocos) {
    for (int i = 0; i < quantidade; i++) {
        printf("Bloco %d (tam %d): ", i, blocos[i]->tamanho_bloco);
        for (int j = 0; j < blocos[i]->tamanho_bloco; j++) {
            printf("%d ", blocos[i]->bloco[j]);
        }
        printf("\n");
    }
}

void imprimir_barris(unsigned int quantidade, barril_t **barris) {
    for (unsigned int i = 0; i < quantidade; i++) {
        printf("barril: %d\n", i);
        for (unsigned int j = 0; j < barris[i]->num_baldes; j++) {
            printf("balde %d: ", j);
            for (unsigned int k = 0; k < barris[i]->baldes[j]->tamanho; k++) {
                printf("%d ", barris[i]->baldes[j]->itens[k]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void imprimir_containeres(unsigned int ntasks, conteiner_t **containeres) {
    for (unsigned int i = 0; i < ntasks; i++) {
        printf("container %d:\n", i);
        for (unsigned int j = 0; j < containeres[i]->tamanho; j++) {
            printf("%d ", containeres[i]->itens[j]);
        }
        printf("\n");
    }
}

int sort_paralelo(unsigned int *vetor, unsigned int tam, unsigned int ntasks, unsigned int nthreads) {
    if (ntasks > tam) ntasks = tam;
    if (nthreads > ntasks) nthreads = ntasks;

    pthread_t threads[nthreads];

    // Fase 1: Particionar o vetor
    vetor_t *subvetores[ntasks];
    bloco_t *particao[ntasks];
    unsigned int resto = tam % ntasks;
    for (unsigned int i = 0; i < ntasks; i++) {
        subvetores[i] = malloc(sizeof(vetor_t));
        subvetores[i]->vetor = vetor;
        subvetores[i]->tamanho_vetor = tam;
        subvetores[i]->num_blocos = ntasks;
        subvetores[i]->indice_bloco = i;
        subvetores[i]->resto = resto;
        // não é a primeira vez da thread computando
        if (i % nthreads != i) {
            // capturando o valor de retorno da execução anterior dessa thread
            pthread_join(threads[i % nthreads], (void *) (particao + i - nthreads));
            free(subvetores[i - nthreads]);
        }
        pthread_create(&threads[i % nthreads], NULL, particionar_vetor, (void *) subvetores[i]);
    }
    for (unsigned int i = ntasks - nthreads; i < ntasks; i++) {
        pthread_join(threads[i % nthreads], (void *) (particao + i));
        free(subvetores[i]);
    }

    // imprimir_blocos(ntasks, particao);

    // Fase 2: Dividir blocos da partição em baldes
    barril_t *barris[ntasks]; // barril := lista com todos os baldes
    for (unsigned int i = 0; i < ntasks; i++) {
        // não é a primeira vez da thread computando
        if (i % nthreads != i) {
            // capturando o valor de retorno da execução anterior dessa thread
            pthread_join(threads[i % nthreads], (void *) (barris + i - nthreads));
        }
        pthread_create(&threads[i % nthreads], NULL, decompor_bloco, (void *) particao[i]);
    }
    for (unsigned int i = ntasks - nthreads; i < ntasks; i++) {
        pthread_join(threads[i % nthreads], (void *) (barris + i));
    }

    // imprimir_barris(ntasks, barris);

    // Fase 3: Aplicar bubble sort em cada balde
    for (unsigned int i = 0; i < ntasks; i++) {
        // não é a primeira vez da thread computando
        if (i % nthreads != i) {
            // capturando o valor de retorno da execução anterior dessa thread
            pthread_join(threads[i % nthreads], NULL);
        }
        pthread_create(&threads[i % nthreads], NULL, ordenar_baldes, (void *) barris[i]);
    }
    for (unsigned int i = ntasks - nthreads; i < ntasks; i++) {
        pthread_join(threads[i % nthreads], NULL);
    }

    // imprimir_barris(ntasks, barris);

    // Fase 4: Mesclar conteúdo dos baldes e armazenar em contêineres
    caixote_t *caixotes[ntasks];
    conteiner_t *containeres[ntasks];
    for (unsigned int i = 0; i < ntasks; i++) {
        caixotes[i] = malloc(sizeof(caixote_t));
        caixotes[i]->barris = barris;
        caixotes[i]->indice_balde = i;
        caixotes[i]->num_barris = ntasks;
        caixotes[i]->tam_vetor = tam;
        // não é a primeira vez da thread computando
        if (i % nthreads != i) {
            // capturando o valor de retorno da execução anterior dessa thread
            pthread_join(threads[i % nthreads], (void *) (containeres + i - nthreads));
            free(caixotes[i - nthreads]);
        }
        pthread_create(&threads[i % nthreads], NULL, conteinerizar, (void *) caixotes[i]);
    }
    for (unsigned int i = ntasks - nthreads; i < ntasks; i++) {
        pthread_join(threads[i % nthreads], (void *) (containeres + i));
        free(caixotes[i]);
    }

    // imprimir_containeres(ntasks, containeres);

    // Fase 5: Aplicar bubble sort em cada contêiner
    for (unsigned int i = 0; i < ntasks; i++) {
        // não é a primeira vez da thread computando
        if (i % nthreads != i) {
            // capturando o valor de retorno da execução anterior dessa thread
            pthread_join(threads[i % nthreads], NULL);
        }
        pthread_create(&threads[i % nthreads], NULL, ordenar_container, (void *) containeres[i]);
    }
    for (unsigned int i = ntasks - nthreads; i < ntasks; i++) {
        pthread_join(threads[i % nthreads], NULL);
    }

    // imprimir_containeres(ntasks, containeres);

    // Fase 6: Concatenar contêineres
    unsigned int inicio = 0;
    vetorizado_t *vetorizados[ntasks];
    for (unsigned int i = 0; i < ntasks; i++) {
        vetorizados[i] = malloc(sizeof(vetorizado_t));
        vetorizados[i]->vetor = vetor;
        vetorizados[i]->container = containeres[i];
        vetorizados[i]->inicio = inicio;
        // não é a primeira vez da thread computando
        if (i % nthreads != i) {
            // capturando o valor de retorno da execução anterior dessa thread
            pthread_join(threads[i % nthreads], NULL);
        }
        pthread_create(&threads[i % nthreads], NULL, vetorizador, (void *) vetorizados[i]);
        inicio += vetorizados[i]->container->tamanho;
    }
    for (unsigned int i = ntasks - nthreads; i < ntasks; i++) {
        pthread_join(threads[i % nthreads], NULL);
    }

    // Limpando a casa
    for (unsigned int i = 0; i < ntasks; i++) {
        free(vetorizados[i]->container->itens);
        free(vetorizados[i]->container);
        free(vetorizados[i]);
        for (unsigned int j = 0; j < barris[i]->num_baldes; j++) {
            free(barris[i]->baldes[j]->itens);
            free(barris[i]->baldes[j]);
        }
        free(barris[i]->baldes);
        free(barris[i]);
    }

    return 0;
}

// Funcao principal de ordenacao. Deve ser implementada com base nas informacoes fornecidas no enunciado do trabalho.

// Os numeros ordenados deverao ser armazenanos no proprio "vetor".

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
