#ifndef AISHELL_H
#define AISHELL_H

#pragma once
#include <vector>
#include "Move.h"

using namespace std;


// A new AIShell will be created for every move request.
class AIShell{

public:
	//these represent the values for each piece type.
	static const int AI_PIECE=1;
	static const int HUMAN_PIECE = -1;
	static const int NO_PIECE=0;

	static const int NORTH = 0;
	static const int NORTH_EAST = 1;
	static const int EAST = 2;
	static const int SOUTH_EAST = 3;


private:
	//Do not alter the values of numRows or numcols.
	//they are used for deallocating the gameState variable.
	int numRows; //the total number of rows in the game state.
	int numCols; //the total number of columns in the game state.
	int **gameState; //a pointer to a two-dimensional array representing the game state.
	bool gravityOn; //this will be true if gravity is turned on. It will be false if gravity is turned off.
	Move lastMove; //this is the move made last by your opponent. If your opponent has not made a move yet (you move first) then this move will hold the value (-1, -1) instead.


public:
	int deadline; //this is how many milliseconds the AI has to make move.
	int k;        // k is the number of pieces a player must get in a row/column/diagonal to win the game. IE in connect 4, this variable would be 4

	AIShell(int numCols, int numRows, bool gravityOn, int** gameState, Move lastMove);
	~AIShell();

	Move miniMaxSearch(); // Use gameState class variable; makeMove calls this

	vector<Move> getActions(); // Take gravityOn and state from class variables

	Move randomSearch(); // Makes random move; makeMove calls this

	//Minimax Utility Methods:
	int miniMaxSearchMaxValue(int depth);

	int miniMaxSearchMinValue(int depth);

	int getMiniMaxUtility();

	/**
	 * Returns the utility of the cell.
	 * if the cell has AI_PIECE it returns AIUtility
	 * if the cell has HUMAN_UTILITY it returns Human Utility
	 * if the cell is connected with either AIPiece or HumanPiece it returns INT_MAX
	 */
	int getUtilityOfACell(int col, int row);


	Move makeMove();
};

#endif //AISHELL_H
