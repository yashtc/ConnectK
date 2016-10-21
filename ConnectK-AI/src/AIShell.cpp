#include "AIShell.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

AIShell::AIShell(int numCols, int numRows, bool gravityOn, int** gameState, Move lastMove)
{
	this->deadline=0;
	this->numRows=numRows;
	this->numCols=numCols;
	this->gravityOn=gravityOn;
	this->gameState=gameState;
	this->lastMove=lastMove;
}


AIShell::~AIShell()
{
	
	//delete the gameState variable.
	for (int i =0; i<numCols; i++){
		delete [] gameState[i];
	}
	delete [] gameState;

}

Move AIShell::randomSearch() {
	bool move_made = false;
		if (this->gravityOn) {
			//if gravity is on, randomly select a column until an empty cell is found
			do {
				int rand_col = rand() % numCols;
				for (int row = 0; row < numRows; row++) {
					if (gameState[rand_col][row] == NO_PIECE){
						Move m(rand_col, row);
						return m;
					}
				}
			} while (!move_made);
		} else {
			//if gravity is off, randomly select a position until an empty cell is found
			do {
				int rand_col = rand() % numCols;
				int rand_row = rand() % numRows;
				if (gameState[rand_col][rand_row] == NO_PIECE){
					Move m(rand_col, rand_row);
					return m;
				}
			} while (!move_made);
		}
		Move m(0, 0);
		return m;
}

vector<Move> AIShell::getActions() {
	vector<Move> possibleMoves;
	for (int col = 0; col < numCols; col++) {
		for (int row = 0; row < numRows; row++) {
			if (gameState[col][row] == NO_PIECE){
				Move m(col, row);
				possibleMoves.push_back(m);
				if (gravityOn) {
					break;
				}
			}
		}
	}
	return possibleMoves;
}

int AIShell::miniMaxSearchMaxValue(int depth) {
	if (depth <= 0) {
		return getMiniMaxUtility();
	}
	int maxValue = INT_MIN;
	vector<Move> possibleMoves = getActions();
	for (Move oneMove : possibleMoves) {
		gameState[oneMove.row][oneMove.col] = AI_PIECE;
		int valueForMove = miniMaxSearchMinValue(depth - 1);
		if (valueForMove > maxValue) {
			maxValue = valueForMove;
		}
		gameState[oneMove.row][oneMove.col] = NO_PIECE;
	}
	return maxValue;
}

int AIShell::miniMaxSearchMinValue(int depth) {
	if (depth <= 0) {
		return getMiniMaxUtility();
	}
	int minValue = INT_MAX;
	vector<Move> possibleMoves = getActions();
	for (Move oneMove : possibleMoves) {
		gameState[oneMove.row][oneMove.col] = HUMAN_PIECE;
		int valueForMove = miniMaxSearchMaxValue(depth - 1);
		if (valueForMove < minValue) {
			minValue = valueForMove;
		}
		gameState[oneMove.row][oneMove.col] = NO_PIECE;
	}
	return minValue;
}

Move AIShell::miniMaxSearch() {
	// for the current game state, get all possible moves Max (i.e. the AI) can make
	// Move maxMove; int maxValue = -Inf (initialization of max's move and the move's value)
	// for each move m of the possible moves:
	//   val = call miniMaxSearchMinValue after applying m to the state
	//   if val > maxValue:
	//      maxValues = val
	//      maxMove = m
	//   revert the move m from the state
	// return maxMove
	int maxDepth = 4;
	vector<Move> possibleMoves = getActions();
	Move maxMove;
	int maxValue = INT_MIN;
	for (Move oneMove : possibleMoves) {
		gameState[oneMove.row][oneMove.col] = AI_PIECE;
		int valueForMove = miniMaxSearchMinValue(maxDepth);
		if (valueForMove > maxValue) {
			maxValue = valueForMove;
			maxMove.col = oneMove.col;
			maxMove.row = oneMove.row;
		}
		gameState[oneMove.row][oneMove.col] = NO_PIECE;
	}
	return maxMove;
}

Move AIShell::makeMove(){
	//this part should be filled in by the student to implement the AI
	//Example of a move could be: Move move(1, 2); //this will make a move at col 1, row 2
	return randomSearch();
	 
}
