#include "AI.h"

char AI::chessChange(char in){

	return BoardState::intToChar(BoardState::charToInt(in));
}

void AI::positionChange(int& posOut,char m0,char m1){
	posOut=((int)(m1-'1'))+((int)(m0-'a'))*8;
}

void AI::positionChange(char& m0,char& m1,int posIn){
	m1=((char)(posIn%8))+'1';
	m0=((char)(posIn/8))+'a';
}

void AI::moveGenerator(struct Move& moveOut){
	std::chrono::high_resolution_clock::time_point start,stop;
	start = std::chrono::high_resolution_clock::now();
	if(step<=1){
		startGameMove(moveOut);
		stop=std::chrono::high_resolution_clock::now();
		remainMilliSecond-=(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count());
		return;
	}else{
		static const int chanceNodeNum=4;
		static const int flipThreshold=1;

		if(state.closedNum[chessColor::red]+state.closedNum[chessColor::black]<chanceNodeNum){
			completeSearchMove(moveOut,countTime());
			stop = std::chrono::high_resolution_clock::now();
			remainMilliSecond-=(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count());
			return;
		}
		if(noFlipMoveNum(state)>flipThreshold){
			noFlipSearchMove(moveOut,countTime());
			stop = std::chrono::high_resolution_clock::now();
			remainMilliSecond-=(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count());
			return;
		}
		chooseOneForFlip(moveOut);

		stop = std::chrono::high_resolution_clock::now();
		remainMilliSecond-=(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count());
		return;
	}

}

void AI::chooseOneForFlip(struct Move& moveOut){
		moveOut.currentPos=chooseFlip();
		moveOut.nextPos=moveOut.currentPos;
		moveOut.dir=chessDirection::up;
		moveOut.currentChess=chessNum::dark;
}

int AI::countTime(){
	if(step<MAX_STEP-10)
		return(int)((1000*totalSecond)/(MAX_STEP));
	return(int)((remainMilliSecond)/20);
}

int AI::chooseFlip(){
	Color enemy=(state.myTurn)?BoardState::flipColor(state.myColor):state.myColor;

	for(int i=0;i<CHESS_NUM;i++){
		int safe=1;
		if(state.board[i]!=chessNum::dark||state.isAttackedByCannon(i,enemy))continue;
		for(int dir=chessDirection::min;dir<=chessDirection::max;dir++){
			int pos=BoardState::goNextPos[(dir<<5)|i];
			if((pos != chessPos::illegal)&&(state.board[pos] != chessNum::dark)&&
				(state.board[pos] != chessNum::empty)&&(BoardState::colorWithoutCheck(state.board[pos])==enemy)){
				if(BoardState::type(state.board[pos])<=chessNum::elephant){
					safe=0;
				}
			}
		}
		if(safe)return i;
	}

	for(int i=0;i<CHESS_NUM;i++)
		if(state.board[i]!=chessNum::dark)return i;
	return chessPos::illegal;
}

void AI::noFlipSearchMove(struct Move& moveOut,int milliseconds){
	std::chrono::high_resolution_clock::time_point start,stop;
	start = std::chrono::high_resolution_clock::now();
	int branch,usingTime;

	struct Move moveList[Move::maxBranch+1];int order[Move::maxBranch+1];
	Color turn=(state.myTurn)?state.myColor:state.flipColor(state.myColor);
	branch=generateMove(state,moveList,turn,order);
	branch+=(state.closedNum[0]+state.closedNum[1]);

	//Iterative Deepening...
	for(int depth=MIN_SEARCH_DEPTH;depth<MAX_SEARCH_DEPTH;depth++){
		fprintf(stderr,"noFlipSearchMove depth %d  ",depth);fflush(stderr);
		noFlipNegaScout(depth,moveOut);
		stop = std::chrono::high_resolution_clock::now();
#ifdef _INFORMATION_
		std::cout<<"depth "<<depth<<"\tusing time >> ";
		std::cout<<(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count())<<std::endl;
#endif
		fprintf(stderr,"time = %d ms...\n",(int)(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count()));
		fflush(stderr);

		usingTime=(int)std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count();
		if(usingTime>=milliseconds||(usingTime*(branch+1)>=milliseconds))
			break;
	}
}

void AI::completeSearchMove(struct Move& moveOut,int milliseconds){
	std::chrono::high_resolution_clock::time_point start,stop;
	start = std::chrono::high_resolution_clock::now();
	int usingTime;
	int branch;

	struct Move moveList[Move::maxBranch+1];int order[Move::maxBranch+1];
	Color turn=(state.myTurn)?state.myColor:state.flipColor(state.myColor);
	branch=generateMove(state,moveList,turn,order);
	branch+=(state.closedNum[0]+state.closedNum[1]);

	//Iterative Deepening...
	for(int depth=MIN_SEARCH_DEPTH;;depth++){
		fprintf(stderr,"completeSearchMove depth %d",depth);fflush(stderr);
		completeNegaScout(depth,moveOut);
		stop = std::chrono::high_resolution_clock::now();
#ifdef _INFORMATION_
		std::cout<<"depth "<<depth<<"\tusing time >> ";
		std::cout<<(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count())<<std::endl;
#endif
		usingTime=std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count();
		fprintf(stderr," using time = %d...\n",usingTime);
		if(usingTime>=milliseconds||(usingTime*(branch+1)>=milliseconds))
			break;
	}
}

Score AI::noFlipNegaScout(int depth,struct Move& moveOut){
#ifdef _USING_TRANS_TABLE_
	#define nega_scout(depthPara) (negaScout1(state,transTable,-1*infMaxScore,infMaxScore,depthPara))
	#define chance_node(index,depthPara) (chanceNodeSearch(state,transTable,moveList[order[index]].currentPos,-1*infMaxScore,infMaxScore,depthPara))
#else
	#define nega_scout(depthPara) (negaScout1(state,-1*infMaxScore,infMaxScore,depthPara))
	#define chance_node(index,depthPara) (chanceNodeSearch(state,moveList[order[index]].currentPos,-1*infMaxScore,infMaxScore,depthPara))
#endif
	static const Score infMaxScore=(Score)10000000;
	struct Move moveList[Move::maxBranch+1];
	int order[Move::maxBranch+1];
	int moveNum,maxIndex;
	Score maxScore,thisScore;

	//generating move...
	moveNum=generateMove(state,moveList,(state.myTurn==0)?state.flipColor(state.myColor):state.myColor,order);

	if(moveNum==0)return (Score)0;

	if(moveList[order[0]].type==moveType::flip){
		maxScore=chance_node(0,depth);
	}
	else{
		state.moveWithoutCheck(moveList[order[0]]);
		state.flipTurn();
		maxScore=-1*nega_scout(depth-1);
		state.flipTurn();
		state.unMove(moveList[order[0]]);
	}
	maxIndex=order[0];

	for(int moveIndex=1;moveIndex<moveNum;moveIndex++){
		if(moveList[order[moveIndex]].type==moveType::flip){
			thisScore=chance_node(moveIndex,depth);
		}
		else{
			state.moveWithoutCheck(moveList[order[moveIndex]]);
			state.flipTurn();
			thisScore=-1*nega_scout(depth-1);
			state.flipTurn();
			state.unMove(moveList[order[moveIndex]]);
		}
		if(thisScore>maxScore){
			maxIndex=order[moveIndex];
			maxScore=thisScore;
		}
	}
	moveOut=moveList[maxIndex];

	return maxScore;

#undef nega_scout
#undef chance_node
}

Score AI::completeNegaScout(int depth,struct Move& moveOut){
#ifdef _USING_TRANS_TABLE_
	#define nega_scout(depthPara) (negaScout2(state,transTable,-1*infMaxScore,infMaxScore,depthPara))
	#define chance_node(index,depthPara) (chanceNodeSearch(state,transTable,moveList[order[index]].currentPos,-1*infMaxScore,infMaxScore,depthPara))
#else
	#define nega_scout(depthPara) (negaScout2(state,-1*infMaxScore,infMaxScore,depthPara))
	#define chance_node(index,depthPara) (chanceNodeSearch(state,moveList[order[index]].currentPos,-1*infMaxScore,infMaxScore,depthPara))
#endif
	static const Score infMaxScore=(Score)10000000;
	struct Move moveList[Move::maxBranch+1];
	int order[Move::maxBranch+1];
	int moveNum,maxIndex;
	Score maxScore,thisScore;

	//generating move...
	moveNum=generateMove(state,moveList,(state.myTurn==0)?state.flipColor(state.myColor):state.myColor,order);

	if(moveList[order[0]].type==moveType::flip){
		maxScore=chance_node(0,depth);
	}
	else{
		state.moveWithoutCheck(moveList[order[0]]);
		state.flipTurn();
		maxScore=-1*nega_scout(depth-1);
		state.flipTurn();
		state.unMove(moveList[order[0]]);
	}
	maxIndex=order[0];

	for(int moveIndex=1;moveIndex<moveNum;moveIndex++){
		if(moveList[order[moveIndex]].type==moveType::flip){
			thisScore=chance_node(moveIndex,depth);
		}
		else{
			state.moveWithoutCheck(moveList[order[moveIndex]]);
			state.flipTurn();
			thisScore=-1*nega_scout(depth-1);
			state.flipTurn();
			state.unMove(moveList[order[moveIndex]]);
		}

		if(thisScore>maxScore){
			maxIndex=order[moveIndex];
			maxScore=thisScore;
		}
	}
	moveOut=moveList[maxIndex];

	return maxScore;
#undef nega_scout
#undef chance_node
}

void AI::startGameMove(struct Move& moveOut){
	if(step==0){
		static const int firstFlipPos=3;
		moveOut.type=moveType::flip;
		moveOut.currentPos=firstFlipPos;
		moveOut.nextPos=firstFlipPos;
		state.myTurn=1;
		return;
	}

	if(step==1){
		Chess firstFlipChess=chessNum::dark;
		int firstFlipPos=0;
		for(int i=0;i<CHESS_NUM;i++){
			if(state.board[i]!=chessNum::dark&&state.board[i]!=chessNum::empty){
				break;
			}
		}
		state.myTurn=1;
		for(int i=0;i<CHESS_NUM;i++){
			if(state.board[i]!=chessNum::dark){
				firstFlipChess=state.board[i];
				firstFlipPos=i;
				break;
			}
		}
		if((BoardState::type(firstFlipChess) == chessNum::king)||
		  (BoardState::type(firstFlipChess) >= chessNum::knight)){
		  //flip aroud enemy's king and chess who is smaller than rook...
			for(int dir=chessDirection::min;dir<=chessDirection::max;dir++){
				if(BoardState::goNextPos[(dir<<5)|firstFlipPos]!=chessPos::illegal){
					moveOut.currentPos=BoardState::goNextPos[(dir<<5)|firstFlipPos];
					moveOut.nextPos=moveOut.currentPos;
					moveOut.type=moveType::flip;
					return;
				}
			}
		}
		else{
			//flip with one chess's gap...
			for(int dir=chessDirection::min;dir<=chessDirection::max;dir++){
				if((BoardState::goNextPos[(dir<<5)|firstFlipPos]!=chessPos::illegal)&&
					(BoardState::goNextPos[(dir<<5)|BoardState::goNextPos[(dir<<5)|firstFlipPos]]!=chessPos::illegal)){
						moveOut.currentPos=BoardState::goNextPos[(dir<<5)|BoardState::goNextPos[(dir<<5)|firstFlipPos]];
						moveOut.nextPos=moveOut.currentPos;
						moveOut.type=moveType::flip;
						return;
				}
			}
		}
	}
}

int AI::move(const struct Move& moveIn){
	struct Move moveTest;moveTest=moveIn;
	if(!state.canMove(moveTest))return 0;
	if((step <= 0)&&(state.myColor != chessColor::red)&&(state.myColor != chessColor::black)){
		//first step,set color...
		state.myColor=BoardState::colorWithoutCheck(moveIn.nextChess);
		if(!state.myTurn)state.myColor=BoardState::flipColor(state.myColor);
	}

	state.moveWithoutCheck(moveIn);
	state.flipTurn();
	step++;
	return 1;
}

AI::AI(int tableSizeBitIn,int totalSecondIn):transTable(tableSizeBitIn){
	totalSecond=totalSecondIn;
	remainMilliSecond=1000*totalSecond;
	step=0;
}