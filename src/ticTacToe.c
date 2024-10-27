#include <stdio.h>

#include "ticTacToe.h"

int gameState[3][3] = { 0 };

//displays the 3X3 game board
void render() {
	int i;
	int j;
	//loops through and converts the integer board into chars
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			switch (gameState[i][j]) {
			case 1:
				printf("X");
				break;
			case 2:
				printf("O");
				break;
			default:
				printf("*");
				break;
			}
		}
		printf("\n");
	}
}

//the turn for the player
int play(int player, int row, int col) {
	//increment for user
	player++;
	//assigns integer value to the board
	gameState[row][col] = player;

	//win condition checks
	//horizontal
	if (gameState[row][0] == gameState[row][1] && gameState[row][0] == gameState[row][2]) {
		return player;
	}
	//vertical
	if (gameState[0][col] == gameState[1][col] && gameState[0][col] == gameState[2][col]) {
		return player;
	}
	//diagonal
	if (gameState[1][1] == player) {
		if (gameState[0][0] == gameState[1][1] && gameState[0][0] == gameState[2][2]) {
			return player;
		}
		if (gameState[0][2] == gameState[1][1] && gameState[0][2] == gameState[2][0]) {
			return player;
		}
	}

	return 0;
}