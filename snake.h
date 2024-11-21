#ifndef SNAKE_H
#define SNAKE_H

// Largura, altura, comprimentos máximo
#define LARGURA_BASE 20
#define ALTURA_BASE 10
#define MAX_COMPRIMENTO 100

// Define controles da Cobra
#define CIMA 1
#define BAIXO 2
#define ESQUERDA 3
#define DIREITA 4

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


#endif
