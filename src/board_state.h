#include "definition.h"
#include <string>
#include <sstream>

#ifndef _BOARD_STATE_H_
#define _BOARD_STATE_H_

struct Move{
	int currentPos;
	int nextPos;
	int dir;
	int type;
	Chess nextChess;
	Chess currentChess;
	struct Move& operator=(const struct Move& in){
		currentPos=in.currentPos;
		nextPos=in.nextPos;
		dir=in.dir;
		type=in.type;
		nextChess=in.nextChess;
		currentChess=in.currentChess;
		return *this;
	};
	std::string toString(){
		std::stringstream ss;ss.str("");ss.clear();
		ss<<"pos("<<currentPos<<") dir=\"";
		if(dir==chessDirection::up)ss<<"^";
		else if(dir==chessDirection::down)ss<<"v";
		else if(dir==chessDirection::left)ss<<"<";
		else if(dir==chessDirection::right)ss<<">";
		else ss<<"X";
		ss<<"\" type=\"";
		if(type==moveType::go)ss<<"go";
		else if(type==moveType::jump)ss<<"jump";
		else if(type==moveType::flip)ss<<"flip";
		else ss<<"????";
		ss<<"\"";
		return ss.str();
	};
	static const int maxBranch=12*4+16*3+4*2+4*4;
};

class BoardState{
public:
	struct PieceList{
		Chess chess;
		int where;
		struct PieceList& operator=(const struct PieceList& pl){chess=pl.chess;where=pl.where;return *this;};
	};
public:
	struct PieceList pieceList[2*16];
	int pieceListIndex[CHESS_NUM];

	Color myColor;
	int myTurn;
	Chess board[CHESS_NUM];
	int closedChess[2*8];
	int movableNum[2];
	Chess movableList[2*8];
	static const int goNextPos[4*32];
private:
	static int jumpTable[4*32*4*4*4*4*4*4*4*4];
public:
	void init(char bdIn[],int closedChessIn[],int myFirstIn=1,Color myColorIn=chessColor::unknown);
	void print()const;
	void printPieceList()const;
	void printBoard()const;
	int isValid()const;
	inline int isOver()const;

	static int charToInt(char chIn);
	static char intToChar(int intIn);
	static inline Color flipColor(Color colorIn);
	inline void flipTurn();

	inline int canMove(int dir,int currentPos,int moveTypeIn,int& nextPos,Chess& nextChess)const;
	inline int canGo(int dir,int currentPos,int& nextPos,Chess& nextChess)const;
	inline int canJump(int dir,int currentPos,int& nextPos,Chess& nextChess)const;

	inline void unMove(int currentPos,int prevPos,Chess prevChess);
	inline void unMove(const struct Move& moveIn);
	inline void flipChessWithoutCheck(int posIn,Chess appearChess);
	inline void goWithoutCheck(int dir,int currentPos);
	inline void jumpWithoutCheck(int dir,int currentPos);
	inline void moveWithoutCheck(const struct Move& moveIn);

	inline int jumpPosition(int dir,int currentPos)const;
	static void countJumpTable(int validTable[]=jumpTable);

	static inline int type(Chess chessIn);
	static inline Color colorWithoutCheck(Chess chessIn);
	static inline int isMovable(Chess chessIn);

	friend void testJumpTable();
private:
	static int jumpRightPosition(int currentPos,int pos[8]);
	static int jumpDownPosition(int currentPos,int pos[4]);
	inline void changePieceList(int currentPos,int nextPos);
	inline void removeFromPieceList(int currentPos);
	inline void addToPieceList(int currentPos,int chessIn);
};

inline void BoardState::moveWithoutCheck(const struct Move& moveIn){
	if(moveIn.type==moveType::go)
		goWithoutCheck(moveIn.dir,moveIn.currentPos);
	else if(moveIn.type==moveType::flip)
		flipChessWithoutCheck(moveIn.currentPos,moveIn.nextPos);
	else//jump
		jumpWithoutCheck(moveIn.dir,moveIn.currentPos);
}

inline void BoardState::addToPieceList(int currentPos,int chessIn){
	int clr=(int)colorWithoutCheck(chessIn);
	int currentIndex=(clr<<4)|movableNum[clr];
	pieceList[currentIndex].where=currentPos;
	pieceList[currentIndex].chess=chessIn;
	pieceListIndex[currentPos]=movableNum[clr];
	movableNum[clr]++;
}

inline void BoardState::changePieceList(int currentPos,int nextPos){
	//need run before board change...
	pieceList[colorWithoutCheck(board[currentPos])<<4|pieceListIndex[currentPos]].where=nextPos;
	pieceListIndex[nextPos]=pieceListIndex[currentPos];
	pieceListIndex[currentPos]=chessPos::illegal;
}

inline void BoardState::removeFromPieceList(int currentPos){
	int clr=colorWithoutCheck(board[currentPos]);
	int currentIndex=(clr<<4)|pieceListIndex[currentPos];
	int lastIndex=(clr<<4)|(movableNum[clr]-1);

	pieceList[currentIndex]=pieceList[lastIndex];
	pieceListIndex[pieceList[lastIndex].where]=pieceListIndex[currentPos];
	pieceListIndex[currentPos]=chessPos::illegal;
	movableNum[clr]--;
}

inline int BoardState::canMove(int dir,int currentPos,int moveTypeIn,int& nextPos,Chess& nextChess)const{
//dir must between 0-3,moveType must between 0-1,
//currentPos must between chessPos::min to chessPos::max... 
	if(moveTypeIn==moveType::go)//go
		return canGo(dir,currentPos,nextPos,nextChess);
	else if(moveTypeIn==moveType::flip)//flip
		return (board[currentPos]==chessNum::dark)?1:0;
	//jump..
	return canJump(dir,currentPos,nextPos,nextChess);
}

inline int BoardState::canJump(int dir,int currentPos,int& nextPos,Chess& nextChess)const{
	nextPos=jumpPosition(dir,currentPos);
	if((type(board[currentPos]) != chessNum::cannon)||(nextPos == chessPos::illegal))
		return 0;
	nextChess=board[nextPos];
	return 1;
}

inline int BoardState::canGo(int dir,int currentPos,int& nextPos,Chess& nextChess)const{
//dir must between 0-3,currentPos must between chessPos::min to chessPos::max... 
	static const int validEatTable[16*16]={
		0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,0,
		0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,
		0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,0,
		0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,
		0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
		0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
		0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,
		0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
		1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	nextPos=goNextPos[(dir<<5)|currentPos];
	if(nextPos==chessPos::illegal)return 0;
	nextChess=board[nextPos];
	return validEatTable[ (board[currentPos]<<4) | nextChess ];
}

inline int BoardState::jumpPosition(int dir,int currentPos)const{
	static const int changeTable[16]={
		1,	1,	1,	1,	1,	1,	1,	0,	3,	3,	3,	3,	3,	3,	3,	2
	};
	int ch[8];
	if(dir>chessDirection::down){//horizontal jump...
		for(int i=0;i<8;i++)ch[i]=changeTable[(int)board[i+currentPos-currentPos%8]];
//std::cout<<"after change>>";for(int i=0;i<8;i++)std::cout<<(int)ch[i];std::cout<<std::endl;
		return BoardState::jumpTable[(dir<<21)|(currentPos<<16)|(ch[0]<<14)|(ch[1]<<12)|
			(ch[2]<<10)|(ch[3]<<8)|(ch[4]<<6)|(ch[5]<<4)|(ch[6]<<2)|ch[7]];
	}
	for(int i=0;i<4;i++)ch[i]=changeTable[(int)board[(i<<3)|(currentPos%8)]];
	return BoardState::jumpTable[(dir<<21)|(currentPos<<16)|(ch[0]<<14)|(ch[1]<<12)|
		(ch[2]<<10)|(ch[3]<<8)];
}

inline void BoardState::goWithoutCheck(int dir,int currentPos){
	int nextPos=goNextPos[(dir<<5)|currentPos];
	int nextChess=board[nextPos];
	if(isMovable(nextChess)){
		removeFromPieceList(nextPos);
		movableList[nextChess]--;
	}
	changePieceList(currentPos,nextPos);
	board[nextPos]=board[currentPos];
	board[currentPos]=chessNum::empty;
}

inline void BoardState::unMove(const struct Move& moveIn){
	unMove(moveIn.nextPos,moveIn.currentPos,moveIn.nextChess);
}

inline void BoardState::unMove(int currentPos,int prevPos,Chess prevChess){
	if(currentPos==prevPos){//unflip
		removeFromPieceList(currentPos);
		closedChess[(colorWithoutCheck(board[currentPos])<<3)|type(board[currentPos])]++;
		movableList[board[currentPos]]--;
		board[currentPos]=chessNum::dark;
		return;
	}
	
	changePieceList(currentPos,prevPos);
	board[prevPos]=board[currentPos];
	if(isMovable(prevChess)){
		addToPieceList(currentPos,prevChess);
		movableList[prevChess]++;
	}
	board[currentPos]=prevChess;
}

inline void BoardState::jumpWithoutCheck(int dir,int currentPos){
	int nextPos=jumpPosition(dir,currentPos);
	int nextChess=board[nextPos];

	removeFromPieceList(nextPos);
	changePieceList(currentPos,nextPos);

	movableList[nextChess]--;
	board[nextPos]=board[currentPos];
	board[currentPos]=chessNum::empty;
}

inline void BoardState::flipChessWithoutCheck(int posIn,Chess appearChess){
	board[posIn]=appearChess;
	closedChess[appearChess]--;
	movableList[appearChess]++;
	addToPieceList(posIn,appearChess);
}

inline int BoardState::isMovable(Chess chessIn){
	return ((((int)chessIn)&0b0111)==0b0111)?0:1;
}

inline Color BoardState::colorWithoutCheck(Chess chessIn){
	return (Color)(((int)chessIn)>>3);
}

inline Color BoardState::flipColor(Color colorIn){
#ifdef _WARNING_MESSAGE_
	if(colorIn!=chessColor::red&&colorIn!=chessColor::black)
		std::cout<<"Warning:void BoardState::flipColor()>>input color is unknown..."<<std::endl;
#endif
	return (Color)(((int)colorIn)^0b0001);
}

inline void BoardState::flipTurn(){
#ifdef _WARNING_MESSAGE_
	if(myTurn!=0&&myTurn!=1)std::cout<<"Warning:void BoardState::flipTurn()>>myTurn is unknown..."<<std::endl;
#endif
	myTurn^=0b0001;
}

inline int BoardState::type(Chess chessIn){
	return ((int)chessIn)&0b0111;
}

inline int BoardState::isOver()const{
	if((movableNum[(int)chessColor::red]==0)&&(closedChess[(int)chessColor::red]==0))
		return 1;
	if((movableNum[(int)chessColor::black]==0)&&(closedChess[(int)chessColor::black]==0))
		return 1;
	return 0;
}

void printEatTable();
#endif