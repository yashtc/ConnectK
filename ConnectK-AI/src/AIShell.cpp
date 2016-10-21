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

Move AIShell::makeMove(){
	//this part should be filled in by the student to implement the AI
	//Example of a move could be: Move move(1, 2); //this will make a move at col 1, row 2
	return randomSearch();
	 
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
				if(gameState[c][r] == currentPiece){
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
