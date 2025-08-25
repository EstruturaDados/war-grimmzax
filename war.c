#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>


// --- Constantes Globais ---
#define TAM_MAX_NOME 30
#define TAM_MAX_COR 10
#define TAM_MAX_MISSAO 100
#define TOTAL_MISSOES 5

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
void liberarMemoria(Territorio* mapa, char* missaoJogador);

// Interface com o usuário
void exibirMapa(const Territorio* mapa, int numTerritorios);
void exibirMenu(void);
void exibirMissao(const char* missao);

// Lógica principal do jogo
void atacar(Territorio* atacante, Territorio* defensor);
int escolherTerritorio(const Territorio* mapa, int numTerritorios, const char* cor, int modo);
void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
int verificarMissao(const char* missao, Territorio* mapa, int tamanho);

// Utilitários
void limparBufferEntrada(void);

// --- Função Principal ---
int main() {
    setlocale(LC_ALL, "Portuguese");
    srand((unsigned)time(NULL));

    // Vetor de missões pré-definidas
    char* missoes[TOTAL_MISSOES] = {
        "Conquistar 3 territórios seguidos",
        "Eliminar todas as tropas da cor vermelha",
        "Ter pelo menos 10 tropas em um território",
        "Conquistar pelo menos 2 territórios da cor vermelha",
        "Possuir todos os territórios com número ímpar"
    };

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

    // Aloca dinamicamente a missão do jogador
    char* missaoJogador = (char*)malloc(TAM_MAX_MISSAO * sizeof(char));
    if (!missaoJogador) {
        printf("Erro ao alocar memória para a missão!\n");
        liberarMemoria(mapa, NULL);
        return 1;
    }
    atribuirMissao(missaoJogador, missoes, TOTAL_MISSOES);
    printf("\nSUA MISSÃO: ");
    exibirMissao(missaoJogador);

    int opcao;
    int venceu = 0;
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

        // Verifica silenciosamente se a missão foi cumprida
        venceu = verificarMissao(missaoJogador, mapa, numTerritorios);
        if (venceu) {
            printf("\nParabéns! Você cumpriu sua missão: ");
            exibirMissao(missaoJogador);
            break;
        }
    } while (opcao != 0);

    liberarMemoria(mapa, missaoJogador);
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

// Exibe a missão do jogador
void exibirMissao(const char* missao) {
    printf("%s\n", missao);
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

// Sorteia e atribui uma missão ao jogador
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int sorteio = rand() % totalMissoes;
    strcpy(destino, missoes[sorteio]);
}

// Verifica se a missão foi cumprida (lógica simples para cada missão)
int verificarMissao(const char* missao, Territorio* mapa, int tamanho) {
    if (strcmp(missao, "Conquistar 3 territórios seguidos") == 0) {
        // Procura 3 territórios consecutivos da cor Azul
        int cont = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "Azul") == 0) cont++;
            else cont = 0;
            if (cont >= 3) return 1;
        }
    } else if (strcmp(missao, "Eliminar todas as tropas da cor vermelha") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "Vermelho") == 0 && mapa[i].tropas > 0)
                return 0;
        }
        return 1;
    } else if (strcmp(missao, "Ter pelo menos 10 tropas em um território") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "Azul") == 0 && mapa[i].tropas >= 10)
                return 1;
        }
    } else if (strcmp(missao, "Conquistar pelo menos 2 territórios da cor vermelha") == 0) {
        int conquistados = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "Azul") == 0) conquistados++;
        }
        return conquistados >= 2;
    } else if (strcmp(missao, "Possuir todos os territórios com número ímpar") == 0) {
        for (int i = 1; i < tamanho; i += 2) {
            if (strcmp(mapa[i].cor, "Azul") != 0)
                return 0;
        }
        return 1;
    }
    return 0;
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

// Libera a memória alocada para o mapa e missão
void liberarMemoria(Territorio* mapa, char* missaoJogador) {
    if (mapa) free(mapa);
    if (missaoJogador) free(missaoJogador);
}

// Limpa o buffer de entrada
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}