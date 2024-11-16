#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LARGURA_BASE 20
#define ALTURA_BASE 10
#define MAX_COMPRIMENTO 100

#define CIMA 1
#define BAIXO 2
#define ESQUERDA 3
#define DIREITA 4

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h> // Para Sleep() no Windows
#else
    #include <unistd.h> // Para usleep() no Linux/UNIX
    #include <termios.h>
    #include <fcntl.h>
#endif

typedef struct {
    int x;
    int y;
} Posicao;

typedef struct {
    int dificuldade;
    char nome[50];
    int total_obstaculos;
    int largura;
    int altura;
    int velocidade;
} Fase;

typedef struct {
    Posicao corpo[MAX_COMPRIMENTO];
    int comprimento;
    int direcao;
    char nome[50];
} Cobra;

typedef struct {
    Fase fase_atual;
    Cobra cobra;
    Posicao comida;
    int pontos;
    int terminou;
} Jogo;

void limpar_tela();
void desenhar_tabuleiro(Jogo *jogo);
void gerar_comida(Jogo *jogo);
int verificar_colisao(Jogo *jogo);
void mover_cobra(Jogo *jogo);
void iniciar_jogo(Jogo *jogo, Fase fase);
void controle(Jogo *jogo);
void salvar_estatisticas(Jogo *jogo);
void exibir_instrucoes();
void game_over(Jogo *jogo);
void exibir_estatisticas();

#ifdef _WIN32
    int tecla_pressionada() {
        return _kbhit() ? _getch() : -1;
    }
#else
    int tecla_pressionada() {
        struct termios oldt, newt;
        int ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if (ch != EOF) {
            ungetc(ch, stdin);
            return ch;
        }

        return -1;
    }
#endif

int main() {
    srand(time(NULL));
    Jogo jogo;
    Fase fases[] = {
        {1, "Muito Facil", 0, LARGURA_BASE, ALTURA_BASE, 200},
        {2, "Facil", 2, LARGURA_BASE + 5, ALTURA_BASE + 3, 150},
        {3, "Medio", 4, LARGURA_BASE + 7, ALTURA_BASE + 5, 100},
        {4, "Dificil", 6, LARGURA_BASE + 10, ALTURA_BASE + 7, 50},
        {5, "Impossivel", 8, 30, 20, 10}
    };

    int opcao, nivel;
    do {
        limpar_tela();
        printf("1. Iniciar Jogo\n2. Estatisticas\n3. Instrucoes\n0. Sair\nEscolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
			case 1:
			    printf("Escolha o nivel (1-5): ");
			    scanf("%d", &nivel);
			    if (nivel >= 1 && nivel <= 5) {
			        printf("Digite o nome da sua cobra: ");
			        getchar(); // Limpa o buffer de entrada
			        fgets(jogo.cobra.nome, sizeof(jogo.cobra.nome), stdin);
			        jogo.cobra.nome[strcspn(jogo.cobra.nome, "\n")] = 0; // Remove o caractere de nova linha
			        iniciar_jogo(&jogo, fases[nivel - 1]);
			    } else {
			        printf("Nivel invalido.\n");
			    }
			    break;

            case 2:
                exibir_estatisticas();
                break;
            case 3:
                exibir_instrucoes();
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}

void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void desenhar_tabuleiro(Jogo *jogo) {
    limpar_tela();
    for (int i = 0; i < jogo->fase_atual.altura; i++) {
        for (int j = 0; j < jogo->fase_atual.largura; j++) {
            if (i == 0 || i == jogo->fase_atual.altura - 1 || j == 0 || j == jogo->fase_atual.largura - 1) {
                printf("#");
            } else if (i == jogo->comida.y && j == jogo->comida.x) {
                printf("@");
            } else {
                int cobra_impressa = 0;
                for (int k = 0; k < jogo->cobra.comprimento; k++) {
                    if (jogo->cobra.corpo[k].x == j && jogo->cobra.corpo[k].y == i) {
                        printf("O");
                        cobra_impressa = 1;
                        break;
                    }
                }
                if (!cobra_impressa) printf(" ");
            }
        }
        printf("\n");
    }
    printf("Nome da cobra: %s  |  Pontos: %d\n", jogo->cobra.nome, jogo->pontos);
}

void gerar_comida(Jogo *jogo) {
    jogo->comida.x = rand() % (jogo->fase_atual.largura - 2) + 1;
    jogo->comida.y = rand() % (jogo->fase_atual.altura - 2) + 1;
}

int verificar_colisao(Jogo *jogo) {
    if (jogo->cobra.corpo[0].x == 0 || jogo->cobra.corpo[0].x == jogo->fase_atual.largura - 1 ||
        jogo->cobra.corpo[0].y == 0 || jogo->cobra.corpo[0].y == jogo->fase_atual.altura - 1) {
        return 1;
    }

    for (int i = 1; i < jogo->cobra.comprimento; i++) {
        if (jogo->cobra.corpo[i].x == jogo->cobra.corpo[0].x && jogo->cobra.corpo[i].y == jogo->cobra.corpo[0].y) {
            return 1;
        }
    }

    if (jogo->cobra.corpo[0].x == jogo->comida.x && jogo->cobra.corpo[0].y == jogo->comida.y) {
        jogo->pontos += 10;
        jogo->cobra.comprimento++;
        gerar_comida(jogo);
    }

    return 0;
}

void mover_cobra(Jogo *jogo) {
    for (int i = jogo->cobra.comprimento - 1; i > 0; i--) {
        jogo->cobra.corpo[i] = jogo->cobra.corpo[i - 1];
    }

    switch (jogo->cobra.direcao) {
        case CIMA: jogo->cobra.corpo[0].y--; break;
        case BAIXO: jogo->cobra.corpo[0].y++; break;
        case ESQUERDA: jogo->cobra.corpo[0].x--; break;
        case DIREITA: jogo->cobra.corpo[0].x++; break;
    }
}

void iniciar_jogo(Jogo *jogo, Fase fase) {
    jogo->fase_atual = fase;
    jogo->cobra.comprimento = 1;
    jogo->cobra.direcao = DIREITA;
    jogo->cobra.corpo[0].x = fase.largura / 2;
    jogo->cobra.corpo[0].y = fase.altura / 2;
    jogo->pontos = 0;
    jogo->terminou = 0;
    gerar_comida(jogo);

    while (!jogo->terminou) {
        desenhar_tabuleiro(jogo);
        controle(jogo);
        mover_cobra(jogo);
        jogo->terminou = verificar_colisao(jogo);

        #ifdef _WIN32
            Sleep(jogo->fase_atual.velocidade);
        #else
            usleep(jogo->fase_atual.velocidade * 1000);
        #endif
    }

    game_over(jogo);
}

void controle(Jogo *jogo) {
    int tecla = tecla_pressionada();
    if (tecla != -1) {
        switch (tecla) {
            case 72: if (jogo->cobra.direcao != BAIXO) jogo->cobra.direcao = CIMA; break;
            case 80: if (jogo->cobra.direcao != CIMA) jogo->cobra.direcao = BAIXO; break;
            case 75: if (jogo->cobra.direcao != DIREITA) jogo->cobra.direcao = ESQUERDA; break;
            case 77: if (jogo->cobra.direcao != ESQUERDA) jogo->cobra.direcao = DIREITA; break;
        }
    }
}

void salvar_estatisticas(Jogo *jogo) {
    FILE *f = fopen("estatisticas.bin", "ab");  // Modo binário de adição
    if (f != NULL) {
        // Grava o nome da cobra e a pontuação no arquivo binário
        fwrite(jogo->cobra.nome, sizeof(jogo->cobra.nome), 1, f);  // Grava o nome da cobra (com 50 bytes)
        fwrite(&jogo->pontos, sizeof(jogo->pontos), 1, f);  // Grava a pontuação
        fclose(f);
    } else {
        printf("Erro ao salvar as estatísticas.\n");
    }
}



void exibir_instrucoes() {
    printf("Instrucoes do Jogo:\nUse as setas do teclado para mover a cobra.\n");
    printf("Evite bater nas paredes e no proprio corpo da cobra.\nComa a comida para crescer e aumentar os pontos.\n");
    printf("Pressione qualquer tecla para voltar...\n");
    getchar();
    getchar();
}

void game_over(Jogo *jogo) {
    printf("\n------------------------------");
    printf("\n\tGAME OVER!");
    printf("\nNome da cobra: %s", jogo->cobra.nome);
    printf("\nPontuacao final: %d", jogo->pontos);
    printf("\n------------------------------\n");
    salvar_estatisticas(jogo);

    int opcao;
    printf("1. Jogar novamente\n0. Sair\nEscolha uma opcao: ");
    scanf("%d", &opcao);

    if (opcao == 1) iniciar_jogo(jogo, jogo->fase_atual);
}

void exibir_estatisticas() {
    FILE *f = fopen("estatisticas.bin", "rb");  // Modo binário de leitura
    if (f == NULL) {
        printf("Nenhuma estatistica encontrada.\n");
        return;
    }

    char nome[50];
    int pontos;

    printf("Estatísticas:\n");

    // Loop de leitura do arquivo binário
    while (fread(nome, sizeof(nome), 1, f) == 1 && fread(&pontos, sizeof(pontos), 1, f) == 1) {
        // Exibe o nome e pontos lidos
        printf("Nome: %s | Pontos: %d\n", nome, pontos);
    }

    fclose(f);
}