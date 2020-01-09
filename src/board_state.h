#include <iostream>

#define _WARNING_MESSAGE_

#ifndef _BOARD_STATE_H_
#define _BOARD_STATE_H_

#define BOARD_LENGTH 8
#define BOARD_WIDTH 4
#define CHESS_NUM BOARD_LENGTH*BOARD_WIDTH
#define PIECE_NUM 7
typedef int Color;
typedef unsigned int Chess;

namespace chessNum{
	static const Chess empty		=(Chess)0b0111;
	static const Chess dark			=(Chess)0b1111;
	namespace red{
		static const Chess king		=(Chess)0b0000;
		static const Chess guard	=(Chess)0b0001;
		static const Chess elephant	=(Chess)0b0010;
		static const Chess rook		=(Chess)0b0011;
		static const Chess knight	=(Chess)0b0100;
		static const Chess cannon	=(Chess)0b0101;
		static const Chess pawn		=(Chess)0b0110;
	};
	namespace black{
		static const Chess king		=(Chess)0b1000;
		static const Chess guard	=(Chess)0b1001;
		static const Chess elephant	=(Chess)0b1010;
		static const Chess rook		=(Chess)0b1011;
		static const Chess knight	=(Chess)0b1100;
		static const Chess cannon	=(Chess)0b1101;
		static const Chess pawn		=(Chess)0b1110;
	};
	static const Chess king		=(Chess)0b0000;
	static const Chess guard	=(Chess)0b0001;
	static const Chess elephant	=(Chess)0b0010;
	static const Chess rook		=(Chess)0b0011;
	static const Chess knight	=(Chess)0b0100;
	static const Chess cannon	=(Chess)0b0101;
	static const Chess pawn		=(Chess)0b0110;
};

namespace chessChar{
	static const char empty='-';
	static const char dark='X';
	namespace red{
		static const char king='K';
		static const char guard='G';
		static const char elephant='M';
		static const char rook='R';
		static const char knight='N';
		static const char cannon='C';
		static const char pawn='P';
	};
	namespace black{
		static const char king='k';
		static const char guard='g';
		static const char elephant='m';
		static const char rook='r';
		static const char knight='n';
		static const char cannon='c';
		static const char pawn='p';
	};
};

namespace chessColor{
	static const Color red			=0b0000;
	static const Color black		=0b0001;
	static const Color unknown		=0b0010;
	static const Color unknown_min	=0b0010;
	static const Color unknown_max	=0b0011;
};

namespace chessPos{
	static const int max=31;
	static const int min=0;
	static const int illegal=32;
};

namespace chessDirection{
	static const int up		=0;
	static const int down	=1;
	static const int left	=2;
	static const int right	=3;
	static const int min	=0;
	static const int max	=3;
};

namespace moveType{
	static const int go=0;
	static const int jump=1;
};

class BoardState{
public:
	struct PieceList{
		Chess chess;
		int where;
		int live;
	}pieceList[2*16];
	struct Move{
		int dir;
		int currentPos;
		int moveType;//go:0,jump:1
	};
public:
	Color myColor;
	int myTurn;
	Chess board[CHESS_NUM];
	int closedChess[2*8];
	int movableNum[2];
	static const int goNextPos[4*32];
private:
	static int jumpTable[4*32*4*4*4*4*4*4*4*4];
	//static const int validEatTable[16*16];
public:
	void init(char bdIn[],int closedChessIn[],int myFirstIn=1,Color myColorIn=chessColor::unknown);
	void print()const;
	void printBoard()const;
	int isValid()const;

	static int charToInt(char chIn);
	static char intToChar(int intIn);
	static inline Color flipColor(Color colorIn);
	inline void flipTurn();
	inline void flipChessWithoutCheck(int posIn,Chess appearChess);

	inline int canMove(int dir,int currentPos,int moveTypeIn)const;
	inline int canGo(int dir,int currentPos)const;
	inline int canJump(int dir,int currentPos)const;

	inline void goWithoutCheck(int dir,int currentPos);
	inline void jumpWithoutCheck(int dir,int currentPos);

	inline int jumpPosition(int dir,int currentPos)const;
	static void countJumpTable(int validTable[]=jumpTable);

	static inline int type(Chess chessIn);
	static inline Color colorWithoutCheck(Chess chessIn);
	static inline int isMovable(Chess chessIn);

	friend void testJumpTable();
private:
	static int jumpRightPosition(int currentPos,int pos[8]);
	static int jumpDownPosition(int currentPos,int pos[4]);
};

inline int BoardState::canMove(int dir,int currentPos,int moveTypeIn)const{
//dir must between 0-3,moveType must between 0-1,
//currentPos must between chessPos::min to chessPos::max... 
	if(moveTypeIn==moveType::go)//go
		return canGo(dir,currentPos);
	//jump..
	return canJump(dir,currentPos);
}

inline int BoardState::canJump(int dir,int currentPos)const{
	if((BoardState::type(board[currentPos]) != chessNum::cannon)||
		(jumpPosition(dir,currentPos) == chessPos::illegal))
		return 0;
	return 1;
}

inline int BoardState::canGo(int dir,int currentPos)const{
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

	int nextPos=goNextPos[(dir<<5)|currentPos];
	if(nextPos==chessPos::illegal)return 0;
	return validEatTable[ (board[currentPos]<<4) | board[nextPos] ];
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
	if(isMovable(board[nextPos]))
		movableNum[BoardState::colorWithoutCheck(board[nextPos])]--;
	board[nextPos]=board[currentPos];
	board[currentPos]=chessNum::empty;
}

inline void BoardState::jumpWithoutCheck(int dir,int currentPos){
	int nextPos=jumpPosition(dir,currentPos);
	movableNum[BoardState::colorWithoutCheck(board[nextPos])]--;
	board[nextPos]=board[currentPos];
	board[currentPos]=chessNum::empty;
}

inline void BoardState::flipChessWithoutCheck(int posIn,Chess appearChess){
	board[posIn]=appearChess;
	closedChess[appearChess]--;
	movableNum[colorWithoutCheck(appearChess)]++;
}

inline int BoardState::isMovable(Chess chessIn){
	return ((((int)chessIn)&0b0111)==0b0111)?0:1;
}

inline Color BoardState::colorWithoutCheck(Chess chessIn){
	return (Color)(((int)chessIn)>>3);
}

inline Color BoardState::flipColor(Color colorIn){
	return colorIn^0b0001;
}

inline void BoardState::flipTurn(){
	myTurn^=0b0001;
}

inline int BoardState::type(Chess chessIn){
	return ((int)chessIn)&0b0111;
}

void printEatTable();
#endif