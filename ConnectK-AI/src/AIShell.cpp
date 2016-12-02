#include "AIShell.h"
#include <stdlib.h>
#include <iostream>
#include <climits>
#include <sys/time.h>
using namespace std;

int AIShell::lastSuccessfulDepthForIDS = 3;

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


void addToMapMin(map<int, vector<Move> > &depthVsMoves, set<Move, CMin> &orderedMoves, int depth){
	vector<Move> v(orderedMoves.begin(), orderedMoves.end());
	depthVsMoves[depth] = v;
}

void addToMapMax(map<int, vector<Move> > &depthVsMoves, set<Move, CMax> &orderedMoves, int depth){
	vector<Move> v(orderedMoves.begin(), orderedMoves.end());
	depthVsMoves[depth] = v;
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
		if (miniMaxIsTerminalState(oneMove)) {
			maxValue = INT_MAX;
			gameState[oneMove.col][oneMove.row] = NO_PIECE;
			break;
		}
		int valueForMove = miniMaxSearchMinValue(depth - 1);
		if (valueForMove > maxValue) {
			maxValue = valueForMove;
		}
		gameState[oneMove.col][oneMove.row] = NO_PIECE;
		if (maxValue == INT_MAX) break;
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
		if (miniMaxIsTerminalState(oneMove)) {
			minValue = INT_MIN;
			gameState[oneMove.col][oneMove.row] = NO_PIECE;
			break;
		}
		int valueForMove = miniMaxSearchMaxValue(depth - 1);
		if (valueForMove < minValue) {
			minValue = valueForMove;
		}
		gameState[oneMove.col][oneMove.row] = NO_PIECE;
		if (minValue == INT_MIN) break;
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
	int maxDepth = gravityOn ? 5 : 3;
	vector<Move> possibleMoves = getActions();
	Move maxMove = possibleMoves[0];
	int maxValue = INT_MIN;
	int numPossibleMoves = possibleMoves.size();
	for (int i = 0; i < numPossibleMoves; i++) {
		Move oneMove = possibleMoves[i];
		gameState[oneMove.col][oneMove.row] = AI_PIECE;
		if (miniMaxIsTerminalState(oneMove)) {
			maxMove.col = oneMove.col;
			maxMove.row = oneMove.row;
			gameState[oneMove.col][oneMove.row] = NO_PIECE;
			break;
		}
		int valueForMove = miniMaxSearchMinValue(maxDepth - 1);
		if (valueForMove > maxValue) {
			maxValue = valueForMove;
			maxMove.col = oneMove.col;
			maxMove.row = oneMove.row;
		}
		gameState[oneMove.col][oneMove.row] = NO_PIECE;
		if (maxValue == INT_MAX) break;
	}
	return maxMove;
}

int AIShell::alphaBetaSearchMaxValue(int depth, int currentDepth, int alpha, int beta,
		long int originalTime, bool *valid, map<int, vector<Move> > &depthVsMoves) {
	if (depth <= 0) {
		return getMiniMaxUtility();
	}
	vector<Move> possibleMoves;
	if(depthVsMoves.find(currentDepth + 1) != depthVsMoves.end()){
		possibleMoves = depthVsMoves[currentDepth + 1];
	}else{
		possibleMoves = getActions();
	}
	int numPossibleMoves = possibleMoves.size();
	if (numPossibleMoves == 0) {
		return 0;
	}
	set<Move, CMax> orderedMoves;
	for (int i = 0; i < numPossibleMoves; i++) {
		Move oneMove = possibleMoves[i];
		gameState[oneMove.col][oneMove.row] = AI_PIECE;
		if (miniMaxIsTerminalState(oneMove)) {
			alpha = INT_MAX;
			gameState[oneMove.col][oneMove.row] = NO_PIECE;
			break;
		}
		int alphaForMove = alphaBetaSearchMinValue(depth - 1, currentDepth + 1, alpha, beta, originalTime, valid, depthVsMoves);
		if (*valid == false) {
			return INT_MIN;
		}
		oneMove.utility = alphaForMove;
		orderedMoves.insert(oneMove);
		struct timeval tp;
		gettimeofday(&tp, NULL);
		long int currentTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
		if (currentTime - originalTime >= deadline) {
			*valid = false;
			return INT_MIN;
		}
		if (alphaForMove > alpha) {
			alpha = alphaForMove;
		}
		gameState[oneMove.col][oneMove.row] = NO_PIECE;
		if (alpha >= beta) {
			addToMapMax(depthVsMoves, orderedMoves, currentDepth + 1);
			return INT_MAX;
		}
		if (alpha == INT_MAX) break;
	}
	addToMapMax(depthVsMoves, orderedMoves, currentDepth + 1);
	return alpha;
}

int AIShell::alphaBetaSearchMinValue(int depth, int currentDepth, int alpha, int beta,
		long int originalTime, bool *valid, map<int, vector<Move> > &depthVsMoves) {

	if (depth <= 0) {
		return getMiniMaxUtility();
	}
	vector<Move> possibleMoves;

	if(depthVsMoves.find(currentDepth + 1) != depthVsMoves.end()){
		possibleMoves = depthVsMoves[currentDepth + 1];
	}else{
		possibleMoves = getActions();
	}

	int numPossibleMoves = possibleMoves.size();
	if (numPossibleMoves == 0) {
		return 0;
	}
	set<Move, CMin> orderedMoves;
	for (int i = 0; i < numPossibleMoves; i++) {
		Move oneMove = possibleMoves[i];
		gameState[oneMove.col][oneMove.row] = HUMAN_PIECE;
		if (miniMaxIsTerminalState(oneMove)) {
			beta = INT_MIN;
			gameState[oneMove.col][oneMove.row] = NO_PIECE;
			break;
		}
		int betaForMove = alphaBetaSearchMaxValue(depth - 1, currentDepth + 1, alpha, beta, originalTime, valid, depthVsMoves);
		if (*valid == false) {
			return INT_MAX;
		}
		oneMove.utility = betaForMove;
		orderedMoves.insert(oneMove);

		struct timeval tp;
		gettimeofday(&tp, NULL);
		long int currentTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
		if (currentTime - originalTime >= deadline) {
			*valid = false;
			return INT_MAX;
		}
		if (betaForMove < beta) {
			beta = betaForMove;
		}
		gameState[oneMove.col][oneMove.row] = NO_PIECE;
		if (alpha >= beta) {
			addToMapMin(depthVsMoves, orderedMoves, currentDepth + 1);
			return INT_MIN;
		}
		if (beta == INT_MIN) break;
	}
	addToMapMin(depthVsMoves, orderedMoves, currentDepth + 1);
	return beta;
}

Move AIShell::alphaBetaSearch(int maxDepth, long int originalTime, map<int, vector<Move> > &depthVsMoves) {
	int alpha = INT_MIN, beta = INT_MAX;
	vector<Move> possibleMoves;
	if(depthVsMoves.find(1) != depthVsMoves.end()){
		possibleMoves = depthVsMoves[1];
	}else{
		possibleMoves = getActions();
	}
	Move maxMove = possibleMoves[0];
	int numPossibleMoves = possibleMoves.size();
	bool valid = true;
	set<Move, CMax> orderedMoves;
	for (int i = 0; i < numPossibleMoves; i++) {
		Move oneMove = possibleMoves[i];
		gameState[oneMove.col][oneMove.row] = AI_PIECE;
		if (miniMaxIsTerminalState(oneMove)) {
			maxMove.col = oneMove.col;
			maxMove.row = oneMove.row;
			gameState[oneMove.col][oneMove.row] = NO_PIECE;
			break;
		}
		int alphaForMove = alphaBetaSearchMinValue(maxDepth - 1, 1, alpha, beta, originalTime, &valid, depthVsMoves);
		if (!valid) {
			Move noMove(-1, -1);
			return noMove;
		}
		oneMove.utility = alphaForMove;
		orderedMoves.insert(oneMove);
		struct timeval tp;
		gettimeofday(&tp, NULL);
		long int currentTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
		if (currentTime - originalTime >= deadline) {
			return Move(-1, -1);
		}
		if (alphaForMove > alpha) {
			alpha = alphaForMove;
			maxMove.col = oneMove.col;
			maxMove.row = oneMove.row;
		}
		gameState[oneMove.col][oneMove.row] = NO_PIECE;
		if (alpha == INT_MAX) break;
	}
	addToMapMax(depthVsMoves, orderedMoves, 1);
	return maxMove;
}

Move AIShell::makeMove(){
	//this part should be filled in by the student to implement the AI
	//Example of a move could be: Move move(1, 2); //this will make a move at col 1, row 2
	return IDSearch();
}

Move AIShell::IDSearch() {
	Move m;
	bool deadlinePassed = false;
	deadline = deadline - 500;

	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int originalTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	map<int, vector<Move> > depthVsMoves;
	int depthToTry = lastSuccessfulDepthForIDS - 1;
	do {
		Move currentMove = alphaBetaSearch(depthToTry, originalTime, depthVsMoves);
		if (currentMove.row == -1) {
			return lastSuccessfulMoveInIDS;
		}
		gettimeofday(&tp, NULL);
		long int currentTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
		int timeElapsed = currentTime - originalTime;

		if (timeElapsed > deadline) {
			deadlinePassed = true;
			//we should write a break statement here instead of setting the parameter to true
		} else {
			m.col = currentMove.col;
			m.row = currentMove.row;
			AIShell::lastSuccessfulDepthForIDS = depthToTry;
			lastSuccessfulMoveInIDS.col = m.col;
			lastSuccessfulMoveInIDS.row = m.row;
			depthToTry++;
		}
	} while (!deadlinePassed);
	return m;
}

Utility AIShell::getUtilityOfACell(int col, int row){
	int currentPiece = gameState[col][row];
	Utility utility = { 0, 0};
	for(int direction = 0; direction < 4; direction++){
		int r = row;
		int c = col;
		int count = 1;
		bool connected = true;
		int tempPiece = NO_PIECE;
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
				if(currentPiece == NO_PIECE){
					connected = false;
					if(gameState[c][r] == NO_PIECE || gameState[c][r] == tempPiece){
						count++;
					}else if(tempPiece == NO_PIECE){
						//This will be run firstime when a AI_PIECE or HUMAN_PIECE is encountered.
						//In this case we will ignore the aberration and just save it as a tempPiece.
						tempPiece = gameState[c][r];
						count++;
					}else{
						break;
					}

				}else{
					if(gameState[c][r] == currentPiece){
						count++;
					}else{
						break;
					}
				}
			}else{
				//if we crossed the game board, break the loop in that direction.
				break;
			}
		}
		if(count == k){
			if(currentPiece == AI_PIECE){
				if(connected){
					utility.AIUtility = INT_MAX;
					return utility;
				}
				utility.AIUtility += 1;
			}else if (currentPiece == HUMAN_PIECE){
				if(connected){
					utility.humanUtility = INT_MAX;
					return utility;
				}
				utility.humanUtility += 1;
			}else{//currentPiece == NO_PIECE
				if(tempPiece == NO_PIECE){
					utility.humanUtility += 1;
					utility.AIUtility += 1;
				}else if (tempPiece == HUMAN_PIECE){
					utility.humanUtility += 1;
				}else{//tempPiece == AI_PIECE
					utility.AIUtility += 1;
				}
			}
		}
	}
	return utility;
}

int AIShell::getMiniMaxNorthEastUtility() {
	int AIscore = 0, humanScore = 0;
	for (int col = 0; col <= numCols - k; col++) {
		for (int row = 0; row <= numRows - k; row++) {
			int i = 0;
			bool aiPossible = true, humanPossible = true;
			int extraHuman = 0, extraAI = 0;
			for (; i < k; i++) {
				if (gameState[col + i][row + i] == HUMAN_PIECE) {
					extraHuman++;
					aiPossible = false;
				} else if (gameState[col + i][row + i] == AI_PIECE) {
					extraAI++;
					humanPossible = false;
				}
				if (!aiPossible && !humanPossible) {
					break;
				}
			}
			if (i == k) {
				if (aiPossible) AIscore += 1;// + int(0.1 * extraAI + 0.5);
				if (humanPossible) humanScore += 1;// + int(0.1 * extraHuman + 0.5);
			}
		}
	}
	return AIscore - humanScore;
}

int AIShell::getMiniMaxSouthEastUtility() {
	int AIscore = 0, humanScore = 0;
	for (int col = 0; col <= numCols - k; col++) {
		for (int row = k - 1; row < numRows; row++) {
			int i = 0;
			bool aiPossible = true, humanPossible = true;
			int extraHuman = 0, extraAI = 0;
			for (; i < k; i++) {
				if (gameState[col + i][row - i] == HUMAN_PIECE) {
					extraHuman++;
					aiPossible = false;
				} else if (gameState[col + i][row - i] == AI_PIECE) {
					extraAI++;
					humanPossible = false;
				}
				if (!aiPossible && !humanPossible) {
					break;
				}
			}
			if (i == k) {
				if (aiPossible) AIscore += 1;// + int(0.1 * extraAI + 0.5);
				if (humanPossible) humanScore += 1;// + int(0.1 * extraHuman + 0.5);
			}
		}
	}
	return AIscore - humanScore;
}

int AIShell::getMiniMaxVerticalUtility() {
	int AIscore = 0, humanScore = 0;
	for (int col = 0; col < numCols; col++) {
		for (int row = 0; row <= numRows - k; row++) {
			int i = 0;
			int extraAI = 0;
			for (; i < k; i++) {
				if (gameState[col][row + i] == HUMAN_PIECE) {
					break;
				}
				if (gameState[col][row + i] == AI_PIECE) {
					extraAI++;
				}
			}
			if (i == k) {
				AIscore += 1;// + int(0.1 * extraAI + 0.5);
			} else {
				row = row + i;
			}
		}
		for (int row = 0; row <= numRows - k; row++) {
			int i = 0;
			int extraHuman = 0;
			for (; i < k; i++) {
				if (gameState[col][row + i] == AI_PIECE) {
					break;
				}
				if (gameState[col][row + i] == HUMAN_PIECE) {
					extraHuman++;
				}
			}
			if (i == k) {
				humanScore += 1;// + int(0.1 * extraHuman + 0.5);
			} else {
				row = row + i;
			}
		}
	}
	return AIscore - humanScore;
}

int AIShell::getMiniMaxHorizontalUtility() {
	int AIscore = 0, humanScore = 0, extraAI = 0, extraHuman = 0;
	for (int row = 0; row < numRows; row++) {
		for (int col = 0; col <= numCols - k; col++) {
			int i = 0;
			int extraAI = 0;
			for (; i < k; i++) {
				if (gameState[col + i][row] == HUMAN_PIECE) {
					break;
				}
				if (gameState[col][row + i] == AI_PIECE) {
					extraAI++;
				}
			}
			if (i == k) {
				AIscore += 1;// + int(0.1 * extraAI + 0.5);
			} else {
				col = col + i;
			}
		}
		for (int col = 0; col <= numCols - k; col++) {
			int i = 0;
			for (; i < k; i++) {
				if (gameState[col + i][row] == AI_PIECE) {
					break;
				}
				if (gameState[col][row + i] == HUMAN_PIECE) {
					extraHuman++;
				}
			}
			if (i == k) {
				humanScore += 1;// + int(0.1 * extraHuman + 0.5);
			} else {
				col = col + i;
			}
		}
	}
	return AIscore - humanScore;
}

bool AIShell::miniMaxIsTerminalState(Move m) {
	int player = gameState[m.col][m.row];
	// check horizontal
	int count = 0, i = m.col, j = m.row;
	while (count < k && i >= 0 && gameState[i--][m.row] == player) {
		count++;
	}
	i = m.col + 1;
	while (count < k && i < numCols && gameState[i++][m.row] == player) {
		count++;
	}
	if (count == k) return true;

	// check vertical
	count = 0, i = m.col, j = m.row;
	while (count < k && i >= 0 && gameState[m.col][j--] == player) {
		count++;
	}
	j = m.row + 1;
	while (count < k && j < numRows && gameState[m.col][j++] == player) {
		count++;
	}
	if (count == k) return true;

	// check SW/NE diagonal
	count = 0, i = m.col, j = m.row;
	while (count < k && i >= 0 && j >= 0 && gameState[i--][j--] == player) {
		count++;
	}
	i = m.col + 1; j = m.row + 1;
	while (count < k && i < numCols && j < numRows && gameState[i++][j++] == player) {
		count++;
	}
	if (count == k) return true;

	// check SE/NW diagonal
	count = 0, i = m.col, j = m.row;
	while (count < k && i < numCols && j >= 0 && gameState[i++][j--] == player) {
		count++;
	}
	i = m.col - 1; j = m.row + 1;
	while (count < k && i >= 0 && j < numRows && gameState[i--][j++] == player) {
		count++;
	}
	if (count == k) return true;

	return false;
}

int AIShell::getMiniMaxUtility() {
	return getMiniMaxHorizontalUtility() + getMiniMaxVerticalUtility() + getMiniMaxSouthEastUtility() + getMiniMaxNorthEastUtility();
	int AIUtility = 0;
	int humanUtility = 0;
	for(int row = 0; row < numRows; row++){
		for(int col = 0; col < numCols; col ++){
			Utility utility = getUtilityOfACell(col, row);
			if(utility.AIUtility == INT_MAX){
				return INT_MAX;
			}
			if(utility.humanUtility == INT_MAX){
				return INT_MIN;
			}
			AIUtility += utility.AIUtility;
			humanUtility += utility.humanUtility;
		}
	}
	return AIUtility - humanUtility;
}
