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

Move AIShell::makeMove(){
	//this part should be filled in by the student to implement the AI
	//Example of a move could be: Move move(1, 2); //this will make a move at col 1, row 2
	return randomSearch();
	 
}
