// Bibliotecas inclusas
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <locale.h>
#include "snake.h"

// Largura, altura, comprimentos máximo
#define LARGURA_BASE 20
#define ALTURA_BASE 10
#define MAX_COMPRIMENTO 100

// Define controles da Cobra
#define CIMA 1
#define BAIXO 2
#define ESQUERDA 3
#define DIREITA 4

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif




// Funções retomadas acima do main
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
	setlocale(LC_ALL, "Portuguese"); // Colocar o idioma em Português
    srand(time(NULL)); // Gerar numero aleatório
    Jogo jogo;
    Fase fases[] = {
        {1, "Muito Facil", 0, LARGURA_BASE, ALTURA_BASE, 200},
        {2, "Facil", 2, LARGURA_BASE + 5, ALTURA_BASE + 3, 150},
        {3, "Medio", 4, LARGURA_BASE + 7, ALTURA_BASE + 5, 100},
        {4, "Dificil", 6, LARGURA_BASE + 10, ALTURA_BASE + 7, 50},
        {5, "Impossível", 8, 30, 20, 10}
    };

    int opcao, nivel, chave;
    // Tela principal
    do {    
        limpar_tela();
        printf("-----------------------------------------");
        printf("\n*BEM-VINDO AO JOGO DA COBRA DO JEFFERSON*");
        printf("\n-----------------------------------------");
        printf("\n1. Iniciar Jogo\n2. Estatísticas\n3. Instruções\n0. Sair\nEscolha uma opção: ");
        scanf("%d", &opcao);
		
		
		// Opções 
        switch (opcao) {
			case 1:
				system("cls");
				do{
				    printf("Escolha o nível (1-5): ");
				    scanf("%d", &nivel);
				    if (nivel >= 1 && nivel <= 5) {
				    	system("cls");
				        printf("Digite o nome da sua cobra: ");
				        getchar();
				        fgets(jogo.cobra.nome, sizeof(jogo.cobra.nome), stdin);
				        jogo.cobra.nome[strcspn(jogo.cobra.nome, "\n")] = 0;
				        iniciar_jogo(&jogo, fases[nivel - 1]);
				    } else {
				    	chave++;
				        printf("Nível inválido.\n");
				        getchar();
	        			getchar();
	        			system("cls");
				    }
				} while(chave = 0);
			    break;

            case 2:
            	system("cls");
                exibir_estatisticas();
                break;
            case 3:
            	system("cls");
                exibir_instrucoes();
                break;
            case 0:
                printf("Saindo...\n");
                break;;
            default:
                printf("Opção inválida!\n");
                getchar();
                getchar();
        }
    } while (opcao != 0);

    return 0;
}
// Limpa a tela em diferentes sistemas operacionais
void limpar_tela() { 
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Função para desenhar o tabuleiro
void desenhar_tabuleiro(Jogo *jogo) {
    int i, j, k;
    limpar_tela(); // Limpar a tela
    for (i = 0; i < jogo->fase_atual.altura; i++) { // Desenhando as paredes da fase
        for (j = 0; j < jogo->fase_atual.largura; j++) {
            if (i == 0 || i == jogo->fase_atual.altura - 1 || j == 0 || j == jogo->fase_atual.largura - 1) {
                printf("#"); // Printa as paredes
            } else if (i == jogo->comida.y && j == jogo->comida.x) { // Gera comida aleatoriamente
                printf("@");
            } else { // Aumenta o tamanho da Cobra ao comer 
                int cobra_impressa = 0;
                for (k = 0; k < jogo->cobra.comprimento; k++) {
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
	// Estatísticas
    printf("Nome da cobra: %s  |  Pontos: %d  |  Nível da Fase: %d\n", 
           jogo->cobra.nome, 
           jogo->pontos, 
           jogo->fase_atual.dificuldade);
}

// Função de gerar comida
void gerar_comida(Jogo *jogo) {
    jogo->comida.x = rand() % (jogo->fase_atual.largura - 2) + 1;
    jogo->comida.y = rand() % (jogo->fase_atual.altura - 2) + 1;
}

int verificar_colisao(Jogo *jogo) {
	int i;
	// Verifica a colisão da cobra nas paredes da fase 
    if (jogo->cobra.corpo[0].x == 0 || jogo->cobra.corpo[0].x == jogo->fase_atual.largura - 1 || 
        jogo->cobra.corpo[0].y == 0 || jogo->cobra.corpo[0].y == jogo->fase_atual.altura - 1) {
        return 1;
    }

    for (i = 1; i < jogo->cobra.comprimento; i++) {
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
	int i;
    for (i = jogo->cobra.comprimento - 1; i > 0; i--) {
        jogo->cobra.corpo[i] = jogo->cobra.corpo[i - 1];
    }
	// Controles da cobra no switch
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
    FILE *f = fopen("estatisticas.bin", "ab");
    if (f != NULL) {
        fwrite(&jogo->cobra, sizeof(jogo->cobra), 1, f);
        fwrite(&jogo->pontos, sizeof(jogo->pontos), 1, f);
        fwrite(&jogo->fase_atual.dificuldade, sizeof(int), 1, f); // Salva o nível
        fclose(f);
    } else {
        printf("Erro ao salvar as estatísticas.\n");
    }
}

void exibir_instrucoes() {
	system("cls");
    printf("Instruções do Jogo:\nUse as setas do teclado para mover a cobra.\n");
    printf("Evite bater nas paredes e no próprio corpo da cobra.\nComa a comida para crescer e aumentar os pontos.\n");
    printf("Pressione qualquer tecla para voltar...\n");
    getchar();
    getchar();
}

void game_over(Jogo *jogo) {
    printf("\n------------------------------");
    printf("\n\tGAME OVER!");
    printf("\nNome da cobra: %s", jogo->cobra.nome);
    printf("\nPontuação final: %d", jogo->pontos);
    printf("\n------------------------------\n");
    salvar_estatisticas(jogo);

    int opcao;
    printf("1. Jogar novamente\n0. Sair\nEscolha uma opção: ");
    scanf("%d", &opcao);

    if (opcao == 1) iniciar_jogo(jogo, jogo->fase_atual);
}

void exibir_estatisticas() {
    limpar_tela();
    FILE *f = fopen("estatisticas.bin", "rb");
    if (f == NULL) {
        printf("Nenhuma estatística encontrada.\n");
        printf("Pressione qualquer tecla para voltar...\n");
        getchar();
        getchar();
        return;
    }

    Cobra cobra_temp;
    int pontos_temp, nivel_temp;
    printf("Estatísticas:\n");

    while (fread(&cobra_temp, sizeof(Cobra), 1, f) == 1 &&
           fread(&pontos_temp, sizeof(int), 1, f) == 1 &&
           fread(&nivel_temp, sizeof(int), 1, f) == 1) {
        printf("Nome: %s | Pontos: %d | Nível: %d\n", cobra_temp.nome, pontos_temp, nivel_temp);
    }

    fclose(f);

    int opcao;
    printf("\n1. Excluir estatísticas\n0. Voltar\nEscolha uma opção: ");
    scanf("%d", &opcao);

    if (opcao == 1) {
        if (remove("estatisticas.bin") == 0) {
            printf("\nEstatísticas excluídas com sucesso.\n");
        } else {
            printf("\nErro ao excluir as estatísticas.\n");
        }
        printf("Pressione qualquer tecla para continuar...\n");
        getchar();
        getchar();
    }
}