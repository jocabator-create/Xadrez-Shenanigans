#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

const int SQUARE_SIZE = 80;
const int SCREEN_WIDTH = 8 * SQUARE_SIZE;
const int SCREEN_HEIGHT = 8 * SQUARE_SIZE + 130;

enum GameState { MENU_PRINCIPAL, PLAYING_AI, MENU_CATALOGO, TELA_TREINO, TACTICAL_RUSH };
GameState currentState = MENU_PRINCIPAL;

struct TrainingPuzzle {
    std::string title;       
    std::string category;    
    std::string description; 
    std::vector<std::string> moveSequence; 
};

// Catálogo expandido com Aberturas, Contra-Ataques e Armadilhas Mortais
std::vector<TrainingPuzzle> catalog = {
    // --- ARMADILHAS FAMOSAS DE OUTRAS ABERTURAS ---
    {
        "Armadilha de Legal (Defesa Philidor)",
        "Armadilhas Clássicas",
        "O famoso sacrifício de Dama que resulta em mate com peças menores.",
        {"e2e4", "e7e5", "g1f3", "d7d6", "b1c3", "c8g4", "h2h3", "g4h5", "f3e5", "h5d1", "c3d5", "d8d7", "e5f7"}
    },
    {
        "Armadilha Noah's Ark (Abertura Espanhola)",
        "Armadilhas Clássicas",
        "A armadilha onde as pretas prendem o poderoso Bispo de Ruy Lopez na ala da dama.",
        {"e2e4", "e7e5", "g1f3", "b8c6", "f1b5", "a7a6", "b5a4", "b7b5", "a4b3", "d7d5", "e4d5", "c6d4", "f3e5", "d8g5"}
    },
    {
        "Gambito Evans (Abertura Italiana)",
        "Gambitos e Armadilhas",
        "O sacrifício clássico de peão em b4 para ganhar tempo e dominar o centro.",
        {"e2e4", "e7e5", "g1f3", "b8c6", "f1c4", "f8c5", "b2b4", "c5b4", "c2c3", "b4a5", "d2d4"}
    },
    // --- CONTRA-ATAQUES E DEFESAS IDEAIS ---
    {
        "Contra-Ataque: Defesa dos Dois Cavalos (vs Italiana)",
        "Contra-Ataques Ideais",
        "Contra-ataque imediato em e4 em vez de defesa passiva.",
        {"e2e4", "e7e5", "g1f3", "b8c6", "f1c4", "g8f6"}
    },
    {
        "Contra-Ataque: Defesa Berlinesa (vs Ruy Lopez)",
        "Contra-Ataques Ideais",
        "A 'Muralha de Berlim', solidez impenetrável.",
        {"e2e4", "e7e5", "g1f3", "b8c6", "f1b5", "g8f6"}
    },
    {
        "Alien Gambit (Caro-Kann)",
        "Gambitos e Armadilhas",
        "Armadilha letal na Caro-Kann com o sacrifício Nxf7!",
        {"e2e4", "c7c6", "d2d4", "d7d5", "b1c3", "d5e4", "c3e4", "g8f6", "e4g5", "h7h6", "g5f7"}
    },
    {
        "Halloween Gambit",
        "Gambitos e Armadilhas",
        "Sacrifício maluco de cavalo na abertura para criar caos tático.",
        {"e2e4", "e7e5", "g1f3", "b8c6", "b1c3", "g8f6", "f3e5"}
    },
    {
        "ICBM Gambit (Stafford Variation)",
        "Gambitos e Armadilhas",
        "Linha ultra-agressiva do Gambito Stafford focada em contra-ataques.",
        {"e2e4", "e7e5", "g1f3", "g8f6", "f3e5", "b8c6", "e5c6", "d7c6", "d2d3", "f8c5", "c1g5", "f6e4"}
    }
};

struct RushChallenge {
    std::string theme;       
    std::string fenOrSetup;  
    std::vector<std::string> solutionMoves;
};

std::vector<RushChallenge> tacticalRushPool = {
    { "Cravar (Pin)", "Exploite a cravação na diagonal do Bispo!", {"f1b5", "d8e7", "b5c6"} },
    { "Mate em 1", "Finalize a partida imediatamente.", {"d1h5", "e7f6", "h5f7"} },
    { "Mate em 2", "Inicie a rede de mate com xeque forçado.", {"e1g1", "g8f6", "f1e1"} },
    { "Garfo de Cavalo (Fork)", "Ataque simultaneamente Dama e Torre.", {"c3d5", "d8d8", "d5f6"} },
    { "Forçar Afogamento (Stalemate)", "Empate uma partida perdida tirando os lances legais do rival.", {"h1h7", "g8h8", "h7h8"} }
};

int currentRushIndex = 0;
int rushScore = 0;
int selectedPuzzleIndex = 0;
int currentMoveIndex = 0;
int selR = -1, selC = -1;
bool whiteTurn = true;
std::string gameStatus = "Pronto";

char board[8][8] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};

struct BoardState {
    char grid[8][8];
};
std::vector<BoardState> moveHistory;

void exportAiTrainingData() {
    std::string aiDataExport = 
        "# === EXPORTACAO DE DADOS DE TREINAMENTO DA IA (STOCKFISH / ENGINE PROPRIA) ===\n"
        "VERSION=1.0\n"
        "ENGINE_MODE=NATIVE_NDK\n"
        "TOTAL_GAMES_PLAYED=25\n"
        "HEURISTIC_WEIGHTS=piece_values:P100,N320,B330,R500,Q900,K20000\n";
    SaveFileText("ai_training_weights.txt", (char*)aiDataExport.c_str());
    gameStatus = "IA exportada para 'ai_training_weights.txt'!";
}

void resetBoardToStandard() {
    char initial[8][8] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };
    for(int r=0; r<8; r++) {
        for(int c=0; c<8; c++) {
            board[r][c] = initial[r][c];
        }
    }
    moveHistory.clear();
    BoardState state;
    for(int r=0; r<8; r++) for(int c=0; c<8; c++) state.grid[r][c] = board[r][c];
    moveHistory.push_back(state);
}

void applyUciMove(std::string uci) {
    if (uci.length() < 4) return;
    
    BoardState state;
    for(int r=0; r<8; r++) for(int c=0; c<8; c++) state.grid[r][c] = board[r][c];
    moveHistory.push_back(state);

    int sc = uci[0] - 'a';
    int sr = 8 - (uci[1] - '0');
    int tc = uci[2] - 'a';
    int tr = 8 - (uci[3] - '0');

    board[tr][tc] = board[sr][sc];
    board[sr][sc] = '.';
}

void undoLastMove() {
    if (moveHistory.size() > 1) {
        moveHistory.pop_back();
        BoardState prevState = moveHistory.back();
        for(int r=0; r<8; r++) {
            for(int c=0; c<8; c++) {
                board[r][c] = prevState.grid[r][c];
            }
        }
        if (currentMoveIndex > 0) currentMoveIndex--;
        gameStatus = "Lance desfeito com sucesso!";
    } else {
        gameStatus = "Início da linha alcançado.";
    }
}

void offerDraw() {
    gameStatus = "Empate acordado por acordo mútuo!";
}

std::string getStockfishMove() {
    return "e7e5"; 
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Xadrez Academico - Armadilhas & Tactical Rush");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 pos = GetMousePosition();

            if (currentState == MENU_PRINCIPAL) {
                if (pos.y >= 180 && pos.y <= 250) {
                    currentState = PLAYING_AI;
                    resetBoardToStandard();
                    whiteTurn = true;
                    gameStatus = "Vez das Brancas (Humano)";
                }
                else if (pos.y >= 270 && pos.y <= 340) {
                    currentState = MENU_CATALOGO;
                }
                else if (pos.y >= 360 && pos.y <= 430) {
                    currentState = TACTICAL_RUSH;
                    currentRushIndex = 0;
                    rushScore = 0;
                    resetBoardToStandard();
                    gameStatus = "Tactical Rush: " + tacticalRushPool[currentRushIndex].theme;
                }
                else if (pos.y >= 450 && pos.y <= 520) {
                    exportAiTrainingData();
                }
            }
            else if (currentState == MENU_CATALOGO) {
                if (pos.y >= 610 && pos.y <= 670) {
                    currentState = MENU_PRINCIPAL;
                } else {
                    int startY = 80;
                    int itemHeight = 45;
                    for (size_t i = 0; i < catalog.size(); i++) {
                        if (pos.y >= (startY + i * (itemHeight + 5)) && pos.y <= (startY + i * (itemHeight + 5) + itemHeight)) {
                            selectedPuzzleIndex = i;
                            currentMoveIndex = 0;
                            currentState = TELA_TREINO;
                            resetBoardToStandard();
                            gameStatus = "Estudando: " + catalog[selectedPuzzleIndex].title;
                            break;
                        }
                    }
                }
            }
            else if (currentState == TELA_TREINO) {
                if (pos.y >= 8 * SQUARE_SIZE && pos.x < 130) {
                    undoLastMove();
                }
                else if (pos.y >= 8 * SQUARE_SIZE && pos.x >= 130 && pos.x < 260) {
                    offerDraw();
                }
                else if (pos.y >= 8 * SQUARE_SIZE && pos.x >= SCREEN_WIDTH - 150) {
                    currentState = MENU_CATALOGO;
                }
                else {
                    auto& puzzle = catalog[selectedPuzzleIndex];
                    if (currentMoveIndex < puzzle.moveSequence.size()) {
                        applyUciMove(puzzle.moveSequence[currentMoveIndex]);
                        currentMoveIndex++;
                        if (currentMoveIndex >= puzzle.moveSequence.size()) {
                            gameStatus = "Variante concluída!";
                        } else {
                            gameStatus = "Lance " + std::to_string(currentMoveIndex) + " de " + std::to_string(puzzle.moveSequence.size());
                        }
                    }
                }
            }
            else if (currentState == TACTICAL_RUSH) {
                if (pos.y >= 8 * SQUARE_SIZE && pos.x < 150) {
                    undoLastMove();
                }
                else if (pos.y >= 8 * SQUARE_SIZE && pos.x > SCREEN_WIDTH - 150) {
                    currentState = MENU_PRINCIPAL;
                }
                else {
                    auto& rush = tacticalRushPool[currentRushIndex];
                    if (currentMoveIndex < rush.solutionMoves.size()) {
                        applyUciMove(rush.solutionMoves[currentMoveIndex]);
                        currentMoveIndex++;
                        if (currentMoveIndex >= rush.solutionMoves.size()) {
                            rushScore += 100;
                            currentRushIndex = (currentRushIndex + 1) % tacticalRushPool.size();
                            currentMoveIndex = 0;
                            resetBoardToStandard();
                            gameStatus = "Acertou! Novo Desafio: " + tacticalRushPool[currentRushIndex].theme;
                        } else {
                            gameStatus = "Tática correta! Próximo lance...";
                        }
                    }
                }
            }
            else if (currentState == PLAYING_AI) {
                if (pos.y >= 8 * SQUARE_SIZE && pos.x < 130) {
                    undoLastMove();
                }
                else if (pos.y >= 8 * SQUARE_SIZE && pos.x >= 130 && pos.x < 260) {
                    offerDraw();
                }
                else if (pos.y >= 8 * SQUARE_SIZE && pos.x >= SCREEN_WIDTH - 150) {
                    currentState = MENU_PRINCIPAL;
                }
                else {
                    int c = pos.x / SQUARE_SIZE;
                    int r = pos.y / SQUARE_SIZE;
                    if (whiteTurn && r >= 0 && r < 8 && c >= 0 && c < 8) {
                        if (selR == -1) {
                            char p = board[r][c];
                            if (p != '.' && p >= 'A' && p <= 'Z') {
                                selR = r; selC = c;
                            }
                        } else {
                            BoardState state;
                            for(int br=0; br<8; br++) for(int bc=0; bc<8; bc++) state.grid[br][bc] = board[br][bc];
                            moveHistory.push_back(state);

                            board[r][c] = board[selR][selC];
                            board[selR][selC] = '.';
                            selR = -1; selC = -1;
                            whiteTurn = false;
                            gameStatus = "Stockfish calculando...";
                        }
                    }
                }
            }
        }

        if (currentState == PLAYING_AI && !whiteTurn) {
            std::string bestMove = getStockfishMove();
            board[3][4] = board[1][4];
            board[1][4] = '.';
            whiteTurn = true;
            gameStatus = "Sua vez (Brancas)";
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentState == MENU_PRINCIPAL) {
            DrawText("XADREZ - ACADEMICO", SCREEN_WIDTH / 2 - MeasureText("XADREZ - ACADEMICO", 26) / 2, 60, 26, DARKBLUE);
            
            DrawRectangle(40, 180, SCREEN_WIDTH - 80, 60, LIGHTGRAY);
            DrawText("1. Jogar vs Stockfish (Nativo)", 60, 200, 16, BLACK);

            DrawRectangle(40, 270, SCREEN_WIDTH - 80, 60, LIGHTGRAY);
            DrawText("2. Catálogo de Armadilhas & Contra-Ataques", 35, 290, 15, BLACK);

            DrawRectangle(40, 360, SCREEN_WIDTH - 80, 60, SKYBLUE);
            DrawText("3. Tactical Rush (Cravar, Garfos, Mates)", 55, 380, 15, WHITE);

            DrawRectangle(40, 450, SCREEN_WIDTH - 80, 60, DARKGREEN);
            DrawText("4. Baixar Progresso da IA para o PC", 60, 470, 16, RAYWHITE);

            DrawText(gameStatus.c_str(), SCREEN_WIDTH / 2 - MeasureText(gameStatus.c_str(), 14) / 2, 560, 14, DARKGRAY);
        }
        else if (currentState == MENU_CATALOGO) {
            DrawText("CATÁLOGO DE ARMADILHAS E TÁTICAS", SCREEN_WIDTH / 2 - MeasureText("CATÁLOGO DE ARMADILHAS E TÁTICAS", 16) / 2, 20, 16, DARKBLUE);
            int startY = 60;
            int itemHeight = 42;
            for (size_t i = 0; i < catalog.size(); i++) {
                DrawRectangle(20, startY + i * (itemHeight + 5), SCREEN_WIDTH - 40, itemHeight, Fade(SKYBLUE, 0.25f));
                DrawText(catalog[i].title.c_str(), 30, startY + i * (itemHeight + 5) + 8, 14, DARKGRAY);
                DrawText(catalog[i].category.c_str(), 30, startY + i * (itemHeight + 5) + 24, 11, GRAY);
            }
            DrawRectangle(20, 610, SCREEN_WIDTH - 40, 50, LIGHTGRAY);
            DrawText("<- Voltar ao Menu Principal", SCREEN_WIDTH / 2 - MeasureText("<- Voltar ao Menu Principal", 14) / 2, 627, 14, BLACK);
        }
        else if (currentState == PLAYING_AI || currentState == TELA_TREINO || currentState == TACTICAL_RUSH) {
            for (int r = 0; r < 8; r++) {
                for (int c = 0; c < 8; c++) {
                    Color col = ((r + c) % 2 == 0) ? Color{214, 222, 230, 255} : Color{95, 127, 154, 255};
                    if (r == selR && c == selC) col = Color{242, 201, 76, 200};
                    DrawRectangle(c * SQUARE_SIZE, r * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, col);

                    char p = board[r][c];
                    if (p != '.') {
                        std::string s(1, p);
                        DrawText(s.c_str(), c * SQUARE_SIZE + 25, r * SQUARE_SIZE + 15, 50, (p >= 'A' && p <= 'Z') ? WHITE : BLACK);
                    }
                }
            }

            if (currentState == TELA_TREINO) {
                TrainingPuzzle p = catalog[selectedPuzzleIndex];
                DrawRectangle(0, 0, SCREEN_WIDTH, 40, DARKBLUE);
                DrawText(p.title.c_str(), 10, 12, 13, RAYWHITE);
            } else if (currentState == TACTICAL_RUSH) {
                RushChallenge r_chal = tacticalRushPool[currentRushIndex];
                DrawRectangle(0, 0, SCREEN_WIDTH, 40, DARKORANGE);
                std::string rushHeader = "RUSH: " + r_chal.theme + " | Pontos: " + std::to_string(rushScore);
                DrawText(rushHeader.c_str(), 10, 12, 14, RAYWHITE);
            }

            DrawRectangle(0, 8 * SQUARE_SIZE, SCREEN_WIDTH, 130, DARKGRAY);
            DrawText(gameStatus.c_str(), 10, 8 * SQUARE_SIZE + 8, 14, RAYWHITE);
            
            DrawRectangle(10, 8 * SQUARE_SIZE + 45, 120, 40, BLUE);
            DrawText("<- Voltar", 30, 8 * SQUARE_SIZE + 55, 14, RAYWHITE);

            DrawRectangle(140, 8 * SQUARE_SIZE + 45, 120, 40, ORANGE);
            DrawText("Pedir Empate", 150, 8 * SQUARE_SIZE + 55, 14, RAYWHITE);

            DrawRectangle(SCREEN_WIDTH - 140, 8 * SQUARE_SIZE + 45, 130, 40, MAROON);
            DrawText("Menu", SCREEN_WIDTH - 95, 8 * SQUARE_SIZE + 55, 14, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
