#include <iostream>

#ifndef _BOARD_STATE_H_
#define _BOARD_STATE_H_

#define BOARD_LENGTH 8
#define BOARD_WIDTH 4
#define CHESS_NUM BOARD_LENGTH*BOARD_WIDTH
#define PIECE_NUM 7
typedef int Color;
typedef uint8_t Chess;

namespace chessNum{
	static const Chess empty		=(Chess)0b0000;
	static const Chess dark			=(Chess)0b1000;
	namespace red{
		static const Chess king		=(Chess)0b0001;
		static const Chess guard	=(Chess)0b0010;
		static const Chess elephant	=(Chess)0b0011;
		static const Chess rook		=(Chess)0b0100;
		static const Chess knight	=(Chess)0b0101;
		static const Chess cannon	=(Chess)0b0110;
		static const Chess pawn		=(Chess)0b0111;
	};
	namespace black{
		static const Chess king		=(Chess)0b1001;
		static const Chess guard	=(Chess)0b1010;
		static const Chess elephant	=(Chess)0b1011;
		static const Chess rook		=(Chess)0b1100;
		static const Chess knight	=(Chess)0b1101;
		static const Chess cannon	=(Chess)0b1110;
		static const Chess pawn		=(Chess)0b1111;
	};
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

class BoardState{
public:
	struct PieceList{
		int where;
		int live;
	}pieceList[2*8];
public:
	Color myColor;
	Color turnColor;
	Chess board[CHESS_NUM];
	int closedChess[2*8];
	//int pieceList[2*8];
public:
	inline void init(char bdIn[],int closedChessIn[],Color turnColorIn=chessColor::unknown,Color myColorIn=chessColor::unknown);
	void print();

	static int charToInt(char chIn);
	static char intToChar(int intIn);
	static inline Color flipColor(Color colorIn);
	inline void flipTurn();

	inline int type(Chess chessIn);

};

inline void BoardState::init(char bdIn[],int closedChessIn[],Color turnColorIn,Color myColorIn){
	myColor=myColorIn;
	turnColor=turnColorIn;
	for(int i=0;i<2*8;i++)pieceList[i].live=0;
	for(int i=0;i<CHESS_NUM;i++){
		board[i]=(Chess)charToInt(bdIn[i]);
		pieceList[i].live=1;
	}
	for(int i=0;i<PIECE_NUM;i++)
		closedChess[i]=closedChessIn[i];
}

inline Color BoardState::flipColor(Color colorIn){
	return colorIn^0b0001;
}

inline void BoardState::flipTurn(){
	turnColor=flipColor(turnColor);
}

inline int BoardState::type(Chess chessIn){
	return ((int)chessIn)&0b0111;
}

#endif