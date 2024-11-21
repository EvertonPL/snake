#ifndef FUNCOES_H
#define FUNCOES_H

#include "snake.h"

// Declaração das funções
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

#endif // FUNCOES_H
