#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/* Intercala duas metades já ordenadas: v[ini..meio] e v[meio+1..fim] */
static void intercalar(int32_t *v, int32_t *aux, size_t ini, size_t meio, size_t fim) {
    size_t i = ini, j = meio + 1, k = ini;

    /* Pega sempre o menor entre os dois "primeiros" das metades */
    while (i <= meio && j <= fim) {
        if (v[i] <= v[j]) aux[k++] = v[i++];
        else              aux[k++] = v[j++];
    }
    /* Sobras: só uma das duas metades ainda tem elementos */
    while (i <= meio) aux[k++] = v[i++];
    while (j <= fim)  aux[k++] = v[j++];

    /* Copia o trecho ordenado de volta para o vetor original */
    for (k = ini; k <= fim; k++) v[k] = aux[k];
}

/* Divide ao meio, ordena cada metade (recursão) e intercala */
static void merge_rec(int32_t *v, int32_t *aux, size_t ini, size_t fim) {
    if (ini >= fim) return;                 /* caso base: 0 ou 1 elemento */
    size_t meio = ini + (fim - ini) / 2;    /* evita overflow de (ini+fim)/2 */
    merge_rec(v, aux, ini, meio);
    merge_rec(v, aux, meio + 1, fim);
    intercalar(v, aux, ini, meio, fim);
}

void merge_sort(int32_t *v, size_t n) {
    if (n < 2) return;
    int32_t *aux = malloc(n * sizeof(int32_t));  /* alocado uma única vez */
    if (!aux) { perror("malloc"); exit(1); }
    merge_rec(v, aux, 0, n - 1);
    free(aux);
}

int main(int argc, char **argv) {
    const char *arq = (argc > 1) ? argv[1] : "listaOdenada.txt";
    FILE *f = fopen(arq, "r");
    if (!f) { perror("fopen"); return 1; }

    size_t cap = 1 << 20, n = 0;
    int32_t *v = malloc(cap * sizeof(int32_t));
    int x;
    while (fscanf(f, "%d", &x) == 1) {
        if (n == cap) {
            cap *= 2;
            v = realloc(v, cap * sizeof(int32_t));
            if (!v) { perror("realloc"); return 1; }
        }
        v[n++] = x;
    }
    fclose(f);

    clock_t inicio = clock();
    merge_sort(v, n);
    clock_t fim = clock();

    double seg = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo de ordenação: %f segundos (%f ms)\n", seg, seg * 1000.0);

    for (size_t i = 1; i < n; i++)
        if (v[i - 1] > v[i]) { printf("ERRO: fora de ordem em %zu\n", i); return 1; }
    printf("OK: %zu elementos ordenados.\n", n);

    free(v);
    return 0;
}
