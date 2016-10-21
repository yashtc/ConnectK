#include "AIShell.h"
#include <stdlib.h>
#include <iostream>
#include <climits>
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
	int numPossibleMoves = possibleMoves.size();
	for (int i = 0; i < numPossibleMoves; i++) {
		Move oneMove = possibleMoves[i];
		gameState[oneMove.col][oneMove.row] = AI_PIECE;
		int valueForMove = miniMaxSearchMinValue(depth - 1);
		if (valueForMove > maxValue) {
			maxValue = valueForMove;
		}
		gameState[oneMove.col][oneMove.row] = NO_PIECE;
	}
	return maxValue;
}

int AIShell::miniMaxSearchMinValue(int depth) {
	if (depth <= 0) {
		return getMiniMaxUtility();
	}
	int minValue = INT_MAX;
	vector<Move> possibleMoves = getActions();
	int numPossibleMoves = possibleMoves.size();
	for (int i = 0; i < numPossibleMoves; i++) {
		Move oneMove = possibleMoves[i];
		gameState[oneMove.col][oneMove.row] = HUMAN_PIECE;
		int valueForMove = miniMaxSearchMaxValue(depth - 1);
		if (valueForMove < minValue) {
			minValue = valueForMove;
		}
		gameState[oneMove.col][oneMove.row] = NO_PIECE;
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
	int numPossibleMoves = possibleMoves.size();
	for (int i = 0; i < numPossibleMoves; i++) {
		Move oneMove = possibleMoves[i];
		gameState[oneMove.col][oneMove.row] = AI_PIECE;
		int valueForMove = miniMaxSearchMinValue(maxDepth);
		if (valueForMove > maxValue) {
			maxValue = valueForMove;
			maxMove.col = oneMove.col;
			maxMove.row = oneMove.row;
		}
		gameState[oneMove.col][oneMove.row] = NO_PIECE;
	}
	return maxMove;
}

Move AIShell::makeMove(){
	//this part should be filled in by the student to implement the AI
	//Example of a move could be: Move move(1, 2); //this will make a move at col 1, row 2
	return miniMaxSearch();
	 
}

int AIShell::getUtilityOfACell(int col, int row){
	int currentPiece = gameState[col][row];
	int ret = 0;
	for(int direction = 0; direction < 4; direction++){
		int r = row;
		int c = col;
		int count = 1;
		bool connected = true;
		while(count < k){
			if(direction == NORTH){
				r++;
			}else if (direction == NORTH_EAST){
				r++;
				c++;
			}else if (direction == EAST){
				c++;
			}else{//direction == SOUTH_EAST
				r--;
				c++;
			}
			if(c < numCols && r < numRows && r >= 0){
				if(gameState[c][r] == currentPiece && gameState[c][r] != NO_PIECE ){
					count++;
				}else if(gameState[c][r] == NO_PIECE ){
					count++;
					connected = false;
				}else{
					break;
				}
			}
		}
		if(count == k && connected){
			return INT_MAX;
		}
		if(count == k){
			ret ++;
		}
	}
	return ret;
}

int AIShell::getMiniMaxUtility(){
	int AIUtility = 0;
	int humanUtility = 0;
	for(int row = 0; row < numRows; row++){
		for(int col = 0; col < numCols; col ++){
			int utility = getUtilityOfACell(col, row);
			if(gameState[col][row] == AI_PIECE){
				if(utility == INT_MAX){
					return INT_MAX;
				}
				AIUtility += utility;
			}else if (gameState[col][row] == HUMAN_PIECE){
				if(utility == INT_MAX){
					return INT_MIN;
				}
				humanUtility += utility;
			}else{//gameState[col][row] == NO_PIECE
				humanUtility += utility;
				AIUtility += utility;
			}
		}
	}
	return AIUtility - humanUtility;
}
