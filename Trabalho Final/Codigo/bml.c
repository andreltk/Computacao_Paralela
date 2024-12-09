#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_N 20
#define DEFAULT_STEPS 100

#define EMPTY -1
#define RED   0
#define BLUE  1

// Função para alocar uma grade 2D
int** allocate_grid(int N) {
    int horizontal = N;
    while ( horizontal > 1 && (N % horizontal == 0) ) {
        horizontal--;
    }
    int **grid = malloc(N * sizeof(int*));
    for(int i = 0; i < N; i++) {
        grid[i] = malloc(horizontal * sizeof(int));
    }
    return grid;
}

// Função para liberar a memória da grade 2D
void free_grid(int **grid, int N) {
    for(int i = 0; i < N; i++) {
        free(grid[i]);
    }
    free(grid);
}

// Movimentação dos carros vermelhos (para a direita)
void move_red(int **grid, int N) {
    int **new_grid = allocate_grid(N);
    // Copia a grade para nova grade
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            new_grid[i][j] = grid[i][j];
        }
    }

    // Tentativa de movimento
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == RED) {
                int next_col = (j + 1) % N;
                // Se a próxima célula estiver vazia, move
                if (grid[i][next_col] == EMPTY) {
                    new_grid[i][j] = EMPTY;
                    new_grid[i][next_col] = RED;
                }
            }
        }
    }

    // Atualiza a grade
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = new_grid[i][j];
        }
    }

    free_grid(new_grid, N);
}

// Movimentação dos carros azuis (para cima)
void move_blue(int **grid, int N) {
    int **new_grid = allocate_grid(N);
    // Copia a grade para nova grade
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            new_grid[i][j] = grid[i][j];
        }
    }

    // Tentativa de movimento
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == BLUE) {
                int next_row = (i - 1 + N) % N; // subir significa decremento na linha
                // Se a próxima célula estiver vazia, move
                if (grid[next_row][j] == EMPTY) {
                    new_grid[i][j] = EMPTY;
                    new_grid[next_row][j] = BLUE;
                }
            }
        }
    }

    // Atualiza a grade
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = new_grid[i][j];
        }
    }

    free_grid(new_grid, N);
}

int main(int argc, char *argv[]) {
    int N = DEFAULT_N;
    int STEPS = DEFAULT_STEPS;
    double DENSITY_R = 0.2; // Densidade inicial de carros vermelhos
    double DENSITY_B = 0.2; // Densidade inicial de carros azuis

    // Parse de argumentos de linha de comando
    if (argc >= 2) {
        N = atoi(argv[1]);
        if (N <= 0) {
            fprintf(stderr, "Tamanho da grid N inválido. Usando N=%d\n", DEFAULT_N);
            N = DEFAULT_N;
        }
    }

    if (argc >= 3) {
        STEPS = atoi(argv[2]);
        if (STEPS < 0) {
            fprintf(stderr, "Número de passos STEPS inválido. Usando STEPS=%d\n", DEFAULT_STEPS);
            STEPS = DEFAULT_STEPS;
        }
    }

    // Permitir definir densidades via argumentos
    if (argc >= 5) {
        DENSITY_R = atof(argv[3]);
        DENSITY_B = atof(argv[4]);
        // Verifica se as densidades são válidas
        if (DENSITY_R < 0 || DENSITY_R > 1 || DENSITY_B < 0 || DENSITY_B > 1 || (DENSITY_R + DENSITY_B) > 1) {
            fprintf(stderr, "Densidades inválidas: DENSITY_R=%.2f, DENSITY_B=%.2f. Usando DENSITY_R=%.2f, DENSITY_B=%.2f\n",DENSITY_R, DENSITY_B, 0.3, 0.6);
            DENSITY_R = 0.32;
            DENSITY_B = 0.62;
        }
    }

    // Aloca a grade
    int **grid = allocate_grid(N);

    srand(time(NULL));

    // Inicialização aleatória da grade
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double r = (double)rand() / RAND_MAX;
            if (r < DENSITY_R) {
                grid[i][j] = RED;
            } else if (r < DENSITY_R + DENSITY_B) {
                grid[i][j] = BLUE;
            } else {
                grid[i][j] = EMPTY;
            }
        }
    }

    // Imprime estado inicial (PASSO 0)
    printf("STEP 0\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }

    for (int step = 1; step <= STEPS; step++) {
        move_red(grid, N);
        move_blue(grid, N);

        // Imprime o estado após este passo
        printf("STEP %d\n", step);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", grid[i][j]);
            }
            printf("\n");
        }
    }

    // Libera a memória
    free_grid(grid, N);

    return 0;
}
