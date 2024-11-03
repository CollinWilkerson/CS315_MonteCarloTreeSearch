#include <math.h>

#include "agentB.h"

#define EXPLORATION_CONSTANT 1.41 //define exploration constant for UCB1 formula//

//function to create a new node with a given board state//
Node* create_node(char board[3][3]) {
    Node *node = malloc(sizeof(Node)); //allocate memory for the node//
    node->visits = 0; //initialize visit count to zero//
    node->wins = 0; //initialize win count to zero//
    node->num_children = 0; //initialize child count to zero//

    //copy the board state to the node's board//
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            node->board[i][j] = board[i][j];
    return node; //return the created node//
}

//function to calculate UCB1 score for a child node//
double ucb1(Node *parent, Node *child) {
    if (child->visits == 0) return INFINITY; //return infinity if unvisited//
    return (double)child->wins / child->visits + //calculate average win rate//
           EXPLORATION_CONSTANT * sqrt(log(parent->visits) / child->visits); //add exploration term//
}

//function to select the child node with the highest UCB1 score//
Node* select_best_node(Node *node) {
    Node *best_node = NULL; //initialize best node as NULL//
    double best_ucb1 = -INFINITY; //start with a very low UCB1 score//

    //loop through each child node to find the best one//
    for (int i = 0; i < node->num_children; ++i) {
        double ucb1_score = ucb1(node, node->children[i]); //calculate UCB1 score//
        if (ucb1_score > best_ucb1) { //check if score is higher than current best//
            best_ucb1 = ucb1_score; //update the best UCB1 score//
            best_node = node->children[i]; //update the best node//
        }
    }
    return best_node; //return the best child node//
}

//function to expand a node by adding child nodes for each possible move//
void expand_node(Node *node, char player) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (node->board[i][j] == ' ') { //check for empty cell//
                Node *child = create_node(node->board); //create a new child node//
                child->board[i][j] = player; //place player's move//
                node->children[node->num_children++] = child; //add child to node's children//
            }
        }
    }
}

//function to simulate a random game from the current board state//
char simulate_random_game(char board[3][3], char player) {
    char temp_board[3][3]; //create a temporary board for simulation//

    //copy board to temp_board//
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            temp_board[i][j] = board[i][j];

    char winner = ' '; //initialize winner as empty//
    while (winner == ' ') { //continue until there's a winner//
        int row = rand() % 3; //random row//
        int col = rand() % 3; //random column//
        if (temp_board[row][col] == ' ') { //check if cell is empty//
            temp_board[row][col] = player; //place player's move//
            winner = checkWinner(temp_board); //check for winner//
            player = (player == 'O') ? 'X' : 'O'; //switch player//
        }
    }
    return winner; //return the winner of the simulation//
}

//function to backpropagate the result of the simulation up the tree//
void backpropagate(Node *node, char winner, char player) {
    while (node != NULL) { //traverse back to the root//
        node->visits++; //increment visits count//
        if (winner == player) node->wins++; //increment wins if player won//
        node = node->parent; //move to the parent node//
    }
}

//function for agentB to make a move using MCTS//
void agentB_move(char player) {
    Node *root = create_node(board); //create root node with current board state//

    int iterations = 1000; //define the number of MCTS iterations//
    for (int i = 0; i < iterations; ++i) {
        Node *node = root; //start selection from the root//

        //selection: traverse down to a leaf node//
        while (node->num_children > 0) {
            node = select_best_node(node); //select best child based on UCB1//
        }

        //expansion: add children if node is non-terminal//
        if (checkWinner(node->board) == ' ') {
            expand_node(node, char player); //expand with moves for 'O'//
        }

        //simulation: play a random game to determine outcome//
        char winner = simulate_random_game(node->board, char player);

        //backpropagation: update win/visit counts//
        backpropagate(node, winner, char player);
    }

    // choose the child with the highest win rate as the best move//
    Node *best_move = NULL;
    double best_win_rate = -1.0;
    for (int i = 0; i < root->num_children; ++i) {
        double win_rate = (double)root->children[i]->wins / root->children[i]->visits; //calculate win rate//
        if (win_rate > best_win_rate) { //check if current win rate is the best//
            best_win_rate = win_rate; //update the best win rate//
            best_move = root->children[i]; //update the best move//
        }
    }

    if (best_move) { //apply the best move to the main board//
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                board[i][j] = best_move->board[i][j]; //copy best move's board to main board//
    }

    free(root); //free memory used by the MCTS tree//
}
