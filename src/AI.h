#include "definition.h"
#include "search.h"
#include "board_state.h"
#include <chrono>
#include <random>

#ifndef _AI_H_
#define _AI_H_
class AI{
private:
	static const int MIN_SEARCH_DEPTH=3;
	static const int MAX_SEARCH_DEPTH=100;
	static const int MAX_STEP=60;
private:
	std::random_device rand;
public:
	BoardState state;
	TransTable transTable;
	int step;
	int remainMilliSecond;
	int totalSecond;

public:
	void startGameMove(struct Move& moveOut);
	void completeSearchMove(struct Move& moveOut,int milliseconds);
	void noFlipSearchMove(struct Move& moveOut,int milliseconds);
	void randomMove(struct Move& moveOut);


	int chooseFlip();

	int move(const struct Move& moveIn);
	Score completeNegaScout(int depth,struct Move& moveOut);
	Score noFlipNegaScout(int depth,struct Move& moveOut);
	void chooseOneForFlip(struct Move& moveOut);

	void moveGenerator(struct Move& moveOut);
	int countTime();

	void positionChange(int& posOut,char m0,char m1);
	void positionChange(char& m0,char& m1,int posOut);
	char chessChange(char in);

	AI(int tableSizeBitIn=20,int totalSecondIn=850);
};
#endif