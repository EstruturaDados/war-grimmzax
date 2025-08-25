#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

// --- Constantes Globais ---
#define TAM_MAX_NOME 30
#define TAM_MAX_COR 10

// --- Estrutura de Dados ---
typedef struct {
    char nome[TAM_MAX_NOME];
    char cor[TAM_MAX_COR];
    int tropas;
} Territorio;

// --- Protótipos das Funções ---
// Setup e gerenciamento de memória
Territorio* alocarMapa(int numTerritorios);
void cadastrarTerritorios(Territorio* mapa, int numTerritorios);
void liberarMemoria(Territorio* mapa);

// Interface com o usuário
void exibirMapa(const Territorio* mapa, int numTerritorios);
void exibirMenu(void);

// Lógica principal do jogo
void atacar(Territorio* atacante, Territorio* defensor);
int escolherTerritorio(const Territorio* mapa, int numTerritorios, const char* cor, int modo);

// Utilitários
void limparBufferEntrada(void);

// --- Função Principal ---
int main() {
    setlocale(LC_ALL, "Portuguese");
    srand((unsigned)time(NULL));

    int numTerritorios;
    printf("Digite o número de territórios: ");
    scanf("%d", &numTerritorios);
    limparBufferEntrada();

    Territorio* mapa = alocarMapa(numTerritorios);
    if (!mapa) {
        printf("Erro ao alocar memória para o mapa!\n");
        return 1;
    }

    cadastrarTerritorios(mapa, numTerritorios);

    int opcao;
    do {
        exibirMapa(mapa, numTerritorios);
        exibirMenu();
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBufferEntrada();

        if (opcao == 1) {
            // Escolher atacante
            int idxAtacante = escolherTerritorio(mapa, numTerritorios, "Azul", 1);
            if (idxAtacante == -1) continue;
            // Escolher defensor
            int idxDefensor = escolherTerritorio(mapa, numTerritorios, "Azul", 0);
            if (idxDefensor == -1) continue;
            atacar(&mapa[idxAtacante], &mapa[idxDefensor]);
            printf("\nApós o ataque:\n");
            exibirMapa(mapa, numTerritorios);
        } else if (opcao == 0) {
            printf("Saindo do jogo...\n");
        } else {
            printf("Opção inválida!\n");
        }
    } while (opcao != 0);

    liberarMemoria(mapa);
    return 0;
}

// --- Implementação das Funções ---

// Aloca dinamicamente o vetor de territórios
Territorio* alocarMapa(int numTerritorios) {
    return (Territorio*)calloc(numTerritorios, sizeof(Territorio));
}

// Cadastro dos territórios
void cadastrarTerritorios(Territorio* mapa, int numTerritorios) {
    printf("\nCadastro dos territórios:\n");
    for (int i = 0; i < numTerritorios; i++) {
        printf("Território %d:\n", i);
        printf("  Nome: ");
        fgets(mapa[i].nome, TAM_MAX_NOME, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';
        printf("  Cor do exército (Azul/Vermelho): ");
        fgets(mapa[i].cor, TAM_MAX_COR, stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';
        printf("  Número de tropas: ");
        scanf("%d", &mapa[i].tropas);
        limparBufferEntrada();
    }
}

// Exibe o mapa dos territórios
void exibirMapa(const Territorio* mapa, int numTerritorios) {
    printf("\n%-4s %-30s %-10s %-6s\n", "ID", "Nome", "Cor", "Tropas");
    for (int i = 0; i < numTerritorios; i++) {
        printf("%-4d %-30s %-10s %-6d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

// Exibe o menu principal
void exibirMenu(void) {
    printf("\n=== MENU ===\n");
    printf("1. Atacar\n");
    printf("0. Sair\n");
}

// Função para escolher território atacante ou defensor
// modo=1: atacante (da cor do jogador), modo=0: defensor (de cor diferente)
int escolherTerritorio(const Territorio* mapa, int numTerritorios, const char* cor, int modo) {
    int idx;
    if (modo == 1) {
        printf("Escolha o ID do território ATACANTE (sua cor: %s, mínimo 2 tropas):\n", cor);
        for (int i = 0; i < numTerritorios; i++) {
            if (strcmp(mapa[i].cor, cor) == 0 && mapa[i].tropas >= 2)
                printf("  %d - %s (%d tropas)\n", i, mapa[i].nome, mapa[i].tropas);
        }
    } else {
        printf("Escolha o ID do território DEFENSOR (cor diferente):\n");
        for (int i = 0; i < numTerritorios; i++) {
            if (strcmp(mapa[i].cor, cor) != 0)
                printf("  %d - %s [%s] (%d tropas)\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
        }
    }
    printf("ID: ");
    scanf("%d", &idx);
    limparBufferEntrada();
    if (idx < 0 || idx >= numTerritorios) {
        printf("ID inválido!\n");
        return -1;
    }
    if (modo == 1 && (strcmp(mapa[idx].cor, cor) != 0 || mapa[idx].tropas < 2)) {
        printf("Você só pode atacar a partir de seus próprios territórios com pelo menos 2 tropas!\n");
        return -1;
    }
    if (modo == 0 && strcmp(mapa[idx].cor, cor) == 0) {
        printf("Você não pode atacar um território da sua própria cor!\n");
        return -1;
    }
    return idx;
}

// Simula um ataque entre dois territórios
void atacar(Territorio* atacante, Territorio* defensor) {
    printf("\n%s (%s, %d tropas) ataca %s (%s, %d tropas)!\n",
        atacante->nome, atacante->cor, atacante->tropas,
        defensor->nome, defensor->cor, defensor->tropas);
    int dadoAtacante = (rand() % 6) + 1;
    int dadoDefensor = (rand() % 6) + 1;
    printf("Dado atacante: %d | Dado defensor: %d\n", dadoAtacante, dadoDefensor);
    if (dadoAtacante > dadoDefensor) {
        printf("O atacante venceu! O território defensor muda de dono.\n");
        strcpy(defensor->cor, atacante->cor);
        int tropasTransferidas = atacante->tropas / 2;
        defensor->tropas = tropasTransferidas;
        atacante->tropas -= tropasTransferidas;
        if (atacante->tropas < 1) atacante->tropas = 1;
    } else {
        printf("O defensor resistiu! O atacante perde 1 tropa.\n");
        atacante->tropas--;
        if (atacante->tropas < 1) atacante->tropas = 1;
    }
}

// Libera a memória alocada para o mapa
void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

// Limpa o buffer de entrada
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}