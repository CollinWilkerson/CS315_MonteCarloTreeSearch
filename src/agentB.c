#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "common.h"
#include "agentB.h"

#define EXPLORATION_CONSTANT 1.41

typedef struct Node {
    char state[3][3];
    char player; /* 'X' or 'O' */
    int move_row;
    int move_col;
    int visits;
    double wins;
    struct Node* parent;
    struct Node* children[9];
    int num_children;
} Node;

/* Function prototypes */
static Node* create_node(char state[3][3], char player, int move_row, int move_col, Node* parent);
static void expand_node(Node* node, char agent_player, char opponent_player);
static Node* select_best_child(Node* node);
static char simulate_random_game(Node* node, char agent_player, char opponent_player);
static void backpropagate(Node* node, char winner, char agentPlayer);
static int is_terminal(char state[3][3]);
static char get_winner(char state[3][3]);
static void copy_state(char dest[3][3], char src[3][3]);
static void free_tree(Node* node);

void agentB_move(char player) {
    char agent_player = player;
    char opponent_player = (player == 'X') ? 'O' : 'X';

    Node* root = create_node(board, agent_player, -1, -1, NULL);

    int iterations = 5000; // Increased iterations
    for (int i = 0; i < iterations; ++i) {
        Node* node = root;

        /* Selection */
        while (node->num_children > 0) {
            node = select_best_child(node);
        }

        /* Expansion */
        if (!is_terminal(node->state)) {
            expand_node(node, agent_player, opponent_player);
        }

        /* Simulation */
        char winner = simulate_random_game(node, agent_player, opponent_player);

        /* Backpropagation */
        backpropagate(node, winner, agent_player);
    }

    /* Choose the best move */
    Node* best_child = NULL;
    double best_win_rate = -1.0;
    for (int i = 0; i < root->num_children; ++i) {
        Node* child = root->children[i];
        double win_rate = child->visits > 0 ? child->wins / child->visits : 0.0;
        if (win_rate > best_win_rate) {
            best_win_rate = win_rate;
            best_child = child;
        }
    }

    if (suppressMessages == 0) {
        printf("Agent B is considering %d possible moves.\n", root->num_children);
        if (best_child) {
            printf("Agent B selects move at row %d, column %d with win rate %.2f%%.\n",
                   best_child->move_row, best_child->move_col, best_win_rate * 100);
        } else {
            printf("Agent B failed to select a best move, choosing randomly.\n");
        }
    }

    if (best_child) {
        board[best_child->move_row][best_child->move_col] = player;
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
    free_tree(root);
}

/* Function implementations */

static Node* create_node(char state[3][3], char player, int move_row, int move_col, Node* parent) {
    Node* node = (Node*)malloc(sizeof(Node));
    copy_state(node->state, state);
    node->player = player;
    node->move_row = move_row;
    node->move_col = move_col;
    node->visits = 0;
    node->wins = 0.0;
    node->parent = parent;
    node->num_children = 0;
    return node;
}

static void expand_node(Node* node, char agent_player, char opponent_player) {
    char next_player = (node->player == agent_player) ? opponent_player : agent_player;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (node->state[i][j] == ' ') {
                char new_state[3][3];
                copy_state(new_state, node->state);
                new_state[i][j] = next_player;
                Node* child = create_node(new_state, next_player, i, j, node);
                node->children[node->num_children++] = child;
            }
        }
    }
}

static Node* select_best_child(Node* node) {
    Node* best_child = NULL;
    double best_value = -DBL_MAX;
    for (int i = 0; i < node->num_children; i++) {
        Node* child = node->children[i];
        double win_rate = child->visits > 0 ? child->wins / child->visits : 0.0;
        double ucb1 = win_rate +
            EXPLORATION_CONSTANT * sqrt(log(node->visits + 1) / (child->visits + 1));

        if (ucb1 > best_value) {
            best_value = ucb1;
            best_child = child;
        }
    }
    return best_child;
}

static char simulate_random_game(Node* node, char agent_player, char opponent_player) {
    char sim_state[3][3];
    copy_state(sim_state, node->state);
    char current_player = (node->player == agent_player) ? opponent_player : agent_player;
    char winner;
    while ((winner = get_winner(sim_state)) == ' ') {
        int move_row = -1, move_col = -1;
        /* Find random empty cell */
        int empty_cells[9][2];
        int num_empty = 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (sim_state[i][j] == ' ') {
                    empty_cells[num_empty][0] = i;
                    empty_cells[num_empty][1] = j;
                    num_empty++;
                }
            }
        }
        if (num_empty == 0) break; // Draw
        int rand_index = rand() % num_empty;
        move_row = empty_cells[rand_index][0];
        move_col = empty_cells[rand_index][1];
        sim_state[move_row][move_col] = current_player;
        current_player = (current_player == agent_player) ? opponent_player : agent_player;
    }
    return winner;
}

static void backpropagate(Node* node, char winner, char agentPlayer) {
    Node* current_node = node;
    while (current_node != NULL) {
        current_node->visits++;
        if (winner == agentPlayer) {
            current_node->wins += 1.0;
        } else if (winner == 'D') {
            current_node->wins += 0.5;
        }
        /* No need to add wins if the opponent won */
        current_node = current_node->parent;
    }
}

static int is_terminal(char state[3][3]) {
    return get_winner(state) != ' ';
}

static char get_winner(char state[3][3]) {
    /* Same logic as checkWinner() but operates on a given state */
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

static void copy_state(char dest[3][3], char src[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

static void free_tree(Node* node) {
    for (int i = 0; i < node->num_children; i++) {
        free_tree(node->children[i]);
    }
    free(node);
}
