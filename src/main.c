#include <stdio.h>

#include "ticTacToe.h"

int main() {
	int winner = 0;
	int player = 0;
	int row;
	int col;

	render();
	
	//game runs until someone wins
	while (winner == 0) {
		printf("Enter a row: ");
		scanf("%d", &row);
		printf("Enter a col: ");
		scanf("%d", &col);
		winner = play(player, row, col);
		render();
		player++;
		player = player % 2;
	}
	printf("Player %d is the winner!", winner);
}