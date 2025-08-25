#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

// --- Constantes Globais ---
#define NUM_TERRITORIOS 10
#define NUM_MISSOES 3
#define TAM_MAX_NOME 32
#define TAM_MAX_MISSAO 128

// --- Estrutura de Dados ---
typedef struct {
    char nome[TAM_MAX_NOME];
    char corExercito[16];
    int tropas;
} Territorio;

// --- Protótipos das Funções ---
// Setup e gerenciamento de memória
Territorio* alocarMapa(int numTerritorios);
void inicializarTerritorios(Territorio* mapa, int numTerritorios);
void liberarMemoria(Territorio* mapa);

// Interface com o usuário
void exibirMenuPrincipal(void);
void exibirMapa(const Territorio* mapa, int numTerritorios);
void exibirMissao(int idMissao);

// Lógica principal do jogo
void faseDeAtaque(Territorio* mapa, int numTerritorios, const char* corJogador);
void simularAtaque(Territorio* mapa, int origem, int destino, const char* corJogador);
int sortearMissao(void);
int verificarVitoria(const Territorio* mapa, int numTerritorios, int idMissao, const char* corJogador);

// Utilitários
void limparBufferEntrada(void);

// --- Função Principal ---
int main() {
    setlocale(LC_ALL, "Portuguese");
    srand((unsigned)time(NULL));

    Territorio* mapa = alocarMapa(NUM_TERRITORIOS);
    if (!mapa) {
        printf("Erro ao alocar memória para o mapa!\n");
        return 1;
    }

    inicializarTerritorios(mapa, NUM_TERRITORIOS);

    char corJogador[16] = "Azul"; // Exemplo, pode ser sorteado
    int idMissao = sortearMissao();
    int venceu = 0, opcao;

    do {
        exibirMapa(mapa, NUM_TERRITORIOS);
        exibirMissao(idMissao);
        exibirMenuPrincipal();
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, NUM_TERRITORIOS, corJogador);
                break;
            case 2:
                venceu = verificarVitoria(mapa, NUM_TERRITORIOS, idMissao, corJogador);
                if (venceu)
                    printf("Parabéns! Você cumpriu sua missão!\n");
                else
                    printf("Missão ainda não cumprida.\n");
                break;
            case 0:
                printf("Saindo do jogo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
        if (opcao != 0) {
            printf("Pressione Enter para continuar...");
            getchar();
        }
    } while (opcao != 0 && !venceu);

    liberarMemoria(mapa);
    return 0;
}

// --- Implementação das Funções ---
Territorio* alocarMapa(int numTerritorios) {
    return (Territorio*)calloc(numTerritorios, sizeof(Territorio));
}

void inicializarTerritorios(Territorio* mapa, int numTerritorios) {
    // Exemplo de inicialização
    for (int i = 0; i < numTerritorios; i++) {
        snprintf(mapa[i].nome, TAM_MAX_NOME, "Território %d", i+1);
        strcpy(mapa[i].corExercito, (i % 2 == 0) ? "Azul" : "Vermelho");
        mapa[i].tropas = 3;
    }
}

void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

void exibirMenuPrincipal(void) {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Atacar\n");
    printf("2. Verificar missão\n");
    printf("0. Sair\n");
}

void exibirMapa(const Territorio* mapa, int numTerritorios) {
    printf("\n=== MAPA ===\n");
    printf("%-15s %-10s %-6s\n", "Nome", "Cor", "Tropas");
    for (int i = 0; i < numTerritorios; i++) {
        printf("%-15s %-10s %-6d\n", mapa[i].nome, mapa[i].corExercito, mapa[i].tropas);
    }
}

void exibirMissao(int idMissao) {
    printf("\n=== SUA MISSÃO ===\n");
    switch (idMissao) {
        case 0: printf("Conquiste 5 territórios.\n"); break;
        case 1: printf("Destrua todos os exércitos vermelhos.\n"); break;
        case 2: printf("Tenha pelo menos 20 tropas no total.\n"); break;
        default: printf("Missão desconhecida.\n");
    }
}

void faseDeAtaque(Territorio* mapa, int numTerritorios, const char* corJogador) {
    int origem, destino;
    printf("Digite o número do território de origem (0 a %d): ", numTerritorios-1);
    scanf("%d", &origem);
    printf("Digite o número do território de destino (0 a %d): ", numTerritorios-1);
    scanf("%d", &destino);
    limparBufferEntrada();
    simularAtaque(mapa, origem, destino, corJogador);
}

void simularAtaque(Territorio* mapa, int origem, int destino, const char* corJogador) {
    // Lógica de ataque (exemplo simplificado)
    if (origem < 0 || destino < 0) return;
    if (strcmp(mapa[origem].corExercito, corJogador) != 0) {
        printf("Você só pode atacar a partir de seus próprios territórios!\n");
        return;
    }
    if (mapa[origem].tropas < 2) {
        printf("É preciso pelo menos 2 tropas para atacar!\n");
        return;
    }
    // Simulação de ataque (simplificada)
    int resultado = rand() % 2;
    if (resultado) {
        printf("Você conquistou o território!\n");
        strcpy(mapa[destino].corExercito, corJogador);
        mapa[destino].tropas = 1;
        mapa[origem].tropas--;
    } else {
        printf("Ataque falhou!\n");
        mapa[origem].tropas--;
    }
}

int sortearMissao(void) {
    return rand() % NUM_MISSOES;
}

int verificarVitoria(const Territorio* mapa, int numTerritorios, int idMissao, const char* corJogador) {
    int i, count = 0, totalTropas = 0;
    switch (idMissao) {
        case 0: // Conquistar 5 territórios
            for (i = 0; i < numTerritorios; i++)
                if (strcmp(mapa[i].corExercito, corJogador) == 0) count++;
            return count >= 5;
        case 1: // Destruir todos os vermelhos
            for (i = 0; i < numTerritorios; i++)
                if (strcmp(mapa[i].corExercito, "Vermelho") == 0) return 0;
            return 1;
        case 2: // Ter pelo menos 20 tropas
            for (i = 0; i < numTerritorios; i++)
                if (strcmp(mapa[i].corExercito, corJogador) == 0) totalTropas += mapa[i].tropas;
            return totalTropas >= 20;
        default:
            return 0;
    }
}

void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}