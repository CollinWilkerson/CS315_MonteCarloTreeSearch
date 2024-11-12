#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "agentC.h"

void agentC_move(char player) {
    int i, j;
    do {
        i = rand() % 3;
        j = rand() % 3;
    } while (board[i][j] != ' ');
    board[i][j] = player;

    if (suppressMessages == 0) {
        printf("Agent C is making a random move.\n");
    }
} 