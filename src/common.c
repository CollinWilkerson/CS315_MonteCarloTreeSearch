#include <stdio.h>
#include <stdlib.h>
#include "common.h"

char board[3][3];
int suppressMessages = 0;

void initBoard() {
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            board[i][j] = ' ';
}

void displayBoard() {
    system("cls"); /* Use "cls" instead of "clear" if on Windows */
    printf("\n");
    printf(" %c | %c | %c \n", board[0][0], board[0][1], board[0][2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[1][0], board[1][1], board[1][2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[2][0], board[2][1], board[2][2]);
    printf("\n");
}

char checkWinner() {
    int i;
    /* Check rows and columns */
    for (i = 0; i < 3; i++) {
        /* Check rows */
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ')
            return board[i][0];
        /* Check columns */
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ')
            return board[0][i];
    }
    /* Check diagonals */
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ')
        return board[0][0];
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ')
        return board[0][2];
    /* Check for draw or ongoing game */
    for (i = 0; i < 3; i++) {
        int j;
        for (j = 0; j < 3; j++)
            if (board[i][j] == ' ')
                return ' '; /* Game is ongoing */
    }
    return 'D'; /* Draw */
} 