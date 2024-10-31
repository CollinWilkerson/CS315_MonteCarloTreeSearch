#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include "common.h"
#include "agentA.h"

/* Define constants for MCTS */
#define SIMULATION_ITERATIONS 1000
#define UCB1_CONST 0.7 /* Adjusted value */

/* Node structure for MCTS */
typedef struct Node {
    char state[3][3];
    int player; /* 'X' or 'O' */
    int move_row;
    int move_col;
    int visits;
    int wins;
    struct Node* parent;
    struct Node* children[9];
    int child_count;
} Node;

/* Function prototypes */
static Node* createNode(char state[3][3], int player, int move_row, int move_col, Node* parent);
static void addChild(Node* parent, Node* child);
static Node* selectBestChild(Node* node);
static void expandNode(Node* node);
static char simulatePlayout(Node* node);
static void backpropagate(Node* node, char result);
static int isTerminalState(char state[3][3]);
static int checkWinnerState(char state[3][3]);
static void copyState(char dest[3][3], char src[3][3]);
static void freeTree(Node* node);
static void selectRandomMove(char state[3][3], int *row, int *col);

void agentA_move(char player) {
    Node* root = createNode(board, player, -1, -1, NULL);

    for (int i = 0; i < SIMULATION_ITERATIONS; i++) {
        Node* promisingNode = root;

        /* Selection */
        while (promisingNode->child_count > 0) {
            promisingNode = selectBestChild(promisingNode);
        }

        /* Expansion */
        if (!isTerminalState(promisingNode->state)) {
            expandNode(promisingNode);
            if (promisingNode->child_count > 0) {
                promisingNode = promisingNode->children[rand() % promisingNode->child_count];
            }
        }

        /* Simulation */
        char playoutResult = simulatePlayout(promisingNode);

        /* Backpropagation */
        backpropagate(promisingNode, playoutResult);
    }

    /* Choosing the best move */
    Node* bestChild = NULL;
    double bestWinRate = -1.0;
    for (int i = 0; i < root->child_count; i++) {
        Node* child = root->children[i];
        double winRate = (double)child->wins / (double)child->visits;
        if (winRate > bestWinRate) {
            bestWinRate = winRate;
            bestChild = child;
        }
    }

    if (suppressMessages == 0) {
        printf("AgentA is considering %d possible moves.\n", root->child_count);
        if (bestChild) {
            printf("AgentA selects move at row %d, column %d with win rate %.2f%%.\n",
                bestChild->move_row, bestChild->move_col, bestWinRate * 100);
        } else {
            printf("AgentA failed to select a best move, choosing randomly.\n");
        }
    }

    if (bestChild) {
        board[bestChild->move_row][bestChild->move_col] = player;
    } else {
        /* Fallback to random move */
        int i, j;
        do {
            i = rand() % 3;
            j = rand() % 3;
        } while (board[i][j] != ' ');
        board[i][j] = player;
    }

    /* Free memory */
    freeTree(root);
}

/* Function implementations */

static Node* createNode(char state[3][3], int player, int move_row, int move_col, Node* parent) {
    Node* node = (Node*)malloc(sizeof(Node));
    copyState(node->state, state);
    node->player = player;
    node->move_row = move_row;
    node->move_col = move_col;
    node->visits = 0;
    node->wins = 0;
    node->parent = parent;
    node->child_count = 0;
    return node;
}

static void addChild(Node* parent, Node* child) {
    parent->children[parent->child_count++] = child;
}

static Node* selectBestChild(Node* node) {
    Node* bestChild = NULL;
    double bestValue = -DBL_MAX;
    for (int i = 0; i < node->child_count; i++) {
        Node* child = node->children[i];
        double winRate = (double)child->wins / (double)child->visits;
        double ucbValue = winRate +
            UCB1_CONST * sqrt(log((double)node->visits) / (double)child->visits);

        if (ucbValue > bestValue) {
            bestValue = ucbValue;
            bestChild = child;
        }
    }
    return bestChild;
}

static void expandNode(Node* node) {
    // Switch player for child nodes
    char nextPlayer = (node->player == 'X') ? 'O' : 'X';

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (node->state[i][j] == ' ') {
                char newState[3][3];
                copyState(newState, node->state);
                newState[i][j] = nextPlayer;
                Node* child = createNode(newState, nextPlayer, i, j, node);
                addChild(node, child);
            }
        }
    }
}

static char simulatePlayout(Node* node) {
    char simState[3][3];
    copyState(simState, node->state);
    char currentPlayer = node->player;
    char winner;
    while ((winner = checkWinnerState(simState)) == ' ') {
        int moveRow, moveCol;
        if (findWinningMove(simState, currentPlayer, &moveRow, &moveCol)) {
            // Play the winning move
            simState[moveRow][moveCol] = currentPlayer;
        } else if (findBlockingMove(simState, currentPlayer, &moveRow, &moveCol)) {
            // Block opponent's winning move
            simState[moveRow][moveCol] = currentPlayer;
        } else {
            // Choose a random move (or implement additional heuristics)
            selectRandomMove(simState, &moveRow, &moveCol);
            simState[moveRow][moveCol] = currentPlayer;
        }
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }
    return winner;
}

static void backpropagate(Node* node, char result) {
    Node* currentNode = node;
    char rootPlayer = node->player;

    // Traverse up to find the root player
    while (currentNode->parent != NULL) {
        currentNode = currentNode->parent;
    }
    rootPlayer = currentNode->player;

    currentNode = node;
    while (currentNode != NULL) {
        currentNode->visits++;

        if (result == 'D') {
            currentNode->wins += 0.5;
        } else if (result == rootPlayer) {
            currentNode->wins += 1;
        }

        currentNode = currentNode->parent;
    }
}

static int isTerminalState(char state[3][3]) {
    return checkWinnerState(state) != ' ';
}

static int checkWinnerState(char state[3][3]) {
    /* Similar to checkWinner() but operates on a given state */
    /* Check rows and columns */
    for (int i = 0; i < 3; i++) {
        if (state[i][0] == state[i][1] && state[i][1] == state[i][2] && state[i][0] != ' ')
            return state[i][0];
        if (state[0][i] == state[1][i] && state[1][i] == state[2][i] && state[0][i] != ' ')
            return state[0][i];
    }
    /* Check diagonals */
    if (state[0][0] == state[1][1] && state[1][1] == state[2][2] && state[0][0] != ' ')
        return state[0][0];
    if (state[0][2] == state[1][1] && state[1][1] == state[2][0] && state[0][2] != ' ')
        return state[0][2];
    /* Check for draw or ongoing game */
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (state[i][j] == ' ')
                return ' '; /* Game is ongoing */
        }
    }
    return 'D'; /* Draw */
}

static void copyState(char dest[3][3], char src[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

static void freeTree(Node* node) {
    for (int i = 0; i < node->child_count; i++) {
        freeTree(node->children[i]);
    }
    free(node);
}

int findWinningMove(char state[3][3], char player, int *row, int *col) {
    // Check all empty positions to see if placing a mark there wins the game
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (state[i][j] == ' ') {
                state[i][j] = player;
                if (checkWinnerState(state) == player) {
                    *row = i;
                    *col = j;
                    state[i][j] = ' '; // Undo move
                    return 1;
                }
                state[i][j] = ' '; // Undo move
            }
        }
    }
    return 0;
}

int findBlockingMove(char state[3][3], char player, int *row, int *col) {
    char opponent = (player == 'X') ? 'O' : 'X';
    return findWinningMove(state, opponent, row, col);
}

static void selectRandomMove(char state[3][3], int *row, int *col) {
    // Prioritize center, then corners, then edges
    if (state[1][1] == ' ') {
        *row = 1;
        *col = 1;
        return;
    }
    int preferredMoves[8][2] = {
        {0, 0}, {0, 2}, {2, 0}, {2, 2}, // Corners
        {0, 1}, {1, 0}, {1, 2}, {2, 1}  // Edges
    };
    for (int i = 0; i < 8; i++) {
        int r = preferredMoves[i][0];
        int c = preferredMoves[i][1];
        if (state[r][c] == ' ') {
            *row = r;
            *col = c;
            return;
        }
    }
    // As a fallback, pick any empty cell
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (state[i][j] == ' ') {
                *row = i;
                *col = j;
                return;
            }
        }
    }
}
  