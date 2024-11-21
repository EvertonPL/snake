#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <locale.h>
#include "snake.h"
#include "funcoes.h"


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
