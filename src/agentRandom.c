#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "agentB.h"

void agentRandom_move(char player) {
    /* MCTS Agent B */
    /*
    Selection:
        - Start from root node
        - While node is fully expanded and non-terminal:
            - Select child node based on an alternative policy (e.g., epsilon-greedy)
    Expansion:
        - If node is non-terminal:
            - Add new child nodes for unexplored moves
    Simulation:
        - Simulate playouts using a heuristic policy
    Backpropagation:
        - Update node statistics with different weighting
    */

    /* For now, return a sample move */
    int i, j;
    do {
        i = rand() % 3;
        j = rand() % 3;
    } while (board[i][j] != ' ');
    board[i][j] = player;

    if (suppressMessages == 0) {
        printf("Random Agent is making a move.\n");
    }
} 