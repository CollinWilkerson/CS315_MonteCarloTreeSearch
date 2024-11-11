#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP(ms) usleep((ms) * 1000)
#endif
#include <string.h>

#include "common.h"
#include "agentA.h"
#include "agentRandom.h"
#include "agentB.h"

#define AGENT_A_PLAYER 'X'
#define AGENT_B_PLAYER 'O'
#define AGENT_A 'a'
#define AGNET_RANDOM 'r'

int main() {
	srand(time(NULL));
	int choice;
	printf("Select an option:\n");
	printf("1. Watch a single game\n");
	printf("2. Run & plot multiple games\n");
	printf("3. Play against an MCTS algorithm\n");
	printf("Enter your choice: ");
	scanf("%d", &choice);

	if (choice == 1) {
		initBoard();
		char winner = ' ';
		int turn = rand() % 2; // Randomly select starting player (0 or 1)
		while (winner == ' ') {
			displayBoard();
			if (turn == 0) {
				agentA_move(AGENT_A_PLAYER);
				turn = 1;
			} else {
				agentRandom_move(AGENT_B_PLAYER);
				turn = 0;
			}
			winner = checkWinner();

			SLEEP(300000); // Wait 300ms between moves
		}
		displayBoard();
		if (winner == 'D') {
			printf("It's a draw!\n");
		} else if (winner == AGENT_A_PLAYER) {
			printf("Agent A (Player %c) wins!\n", winner);
		} else if (winner == AGENT_B_PLAYER) {
			printf("Agent B (Player %c) wins!\n", winner);
		}
	} else if (choice == 2) {
		char firstAgent;
		char secondAgent;
		printf("Enter first player agent (a - MCTS agent, b - MCTS agent, r - Random agent): ");
		scanf("%c", &firstAgent);
		printf("Enter second player agent (a - MCTS agent, b - MCTS agent, r - Random agent): ");
		scanf("%c", &secondAgent);

		int numGames;
		printf("Enter the number of games to run: ");
		scanf("%d", &numGames);

		char excludeDrawsChoice;
		printf("Exclude draws from results? (y/n): ");
		scanf(" %c", &excludeDrawsChoice);
		int excludeDraws = (excludeDrawsChoice == 'y' || excludeDrawsChoice == 'Y') ? 1 : 0;

		char suppressMessagesChoice;
		printf("Suppress messages during simulation? (y/n): ");
		scanf(" %c", &suppressMessagesChoice);
		suppressMessages = (suppressMessagesChoice == 'y' || suppressMessagesChoice == 'Y') ? 1 : 0;

		int agentAWins = 0, agentBWins = 0, draws = 0;
		FILE *fp = fopen("results.dat", "w");

		for (int i = 1; i <= numGames; i++) {
			initBoard();
			char winner = ' ';
			int turn = rand() % 2; /* Randomly select starting player (0 or 1) */
			while (winner == ' ') {
				if (turn == 0) {
					move(firstAgent,AGENT_A_PLAYER);
					turn = 1;
				} else {
					move(secondAgent, AGENT_B_PLAYER);
					turn = 0;
				}
				winner = checkWinner();
			}
			if (winner == AGENT_A_PLAYER) {
				agentAWins++;
			} else if (winner == AGENT_B_PLAYER) {
				agentBWins++;
			} else if (winner == 'D') {
				draws++;
			}

			int totalGames = excludeDraws ? (agentAWins + agentBWins) : i;
			float agentASuccessRate = totalGames > 0 ? (float)agentAWins / totalGames : 0.0f;
			float agentBSuccessRate = totalGames > 0 ? (float)agentBWins / totalGames : 0.0f;
			float drawRate = excludeDraws ? 0.0f : (float)draws / i;

			fprintf(fp, "%d %f %f %f\n", i,
					agentASuccessRate,
					agentBSuccessRate,
					drawRate);
		}
		fclose(fp);

		/* change this to the directory of your gnuplot binary!! */
		/*FILE *gnuplotPipe = popen("gnuplot -persistent", "w");*/
		FILE *gnuplotPipe = popen("C:/gnuplot -persistent", "w");
		if (gnuplotPipe) {
			fprintf(gnuplotPipe, "set title 'Agent Success Rates Over Games'\n");
			fprintf(gnuplotPipe, "set xlabel 'Number of Games'\n");
			fprintf(gnuplotPipe, "set ylabel 'Success Rate'\n");
			fprintf(gnuplotPipe, "plot 'results.dat' using 1:2 with lines title 'Agent A', \\\n");
			fprintf(gnuplotPipe, "     'results.dat' using 1:3 with lines title 'Agent B', \\\n");
			fprintf(gnuplotPipe, "     'results.dat' using 1:4 with lines title 'Draws'\n");
			fflush(gnuplotPipe);
			pclose(gnuplotPipe);
		} else {
			printf("Error: Could not open gnuplot.\n");
		}

		printf("Results have been written to results.dat and plotted using gnuplot.\n");

		/* Reset suppressMessages */
		suppressMessages = 0;
	} else if (choice == 3) {
		char opponent;
		printf("Select your opponent ('a' for agentA, 'b' for agentB, 'r' for Random agent): ");
		scanf(" %c", &opponent);

		initBoard();
		char winner = ' ';
		int turn = rand() % 2; // Randomly select starting player (0 or 1)
		while (winner == ' ') {
			displayBoard();
			if (turn == 0) {
				int row, col;
				printf("Enter your move (row and column): ");
				scanf("%d %d", &row, &col);
				if (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ') {
					board[row][col] = 'O'; // Human plays 'O'
					turn = 1;
				} else {
					printf("Invalid move. Try again.\n");
				}
			} else {
				move(opponent, 'X');
				turn = 0;
			}
			winner = checkWinner();
		}
		displayBoard();
		if (winner == 'D') {
			printf("It's a draw!\n");
		} else if (winner == 'X') {
			printf("Computer (%c) wins!\n", winner);
		} else {
			printf("You win!\n");
		}
	} else {
		printf("Invalid choice.\n");
	}

	return 0;
}