#ifndef COMMON_H
#define COMMON_H

extern char board[3][3];
extern int suppressMessages;

void initBoard();
void displayBoard();
char checkWinner();
void move(char agent, char player);
#endif