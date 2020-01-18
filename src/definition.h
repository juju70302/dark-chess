#include <iostream>
#include <cstdio>

#define _USING_TRANS_TABLE_
#define _USING_MOVE_ORDERING_
//#define _WARNING_MESSAGE_
//#define _INFORMATION_

#ifndef _DEFINITION_H_
#define _DEFINITION_H_
#define BOARD_LENGTH 8
#define BOARD_WIDTH 4
#define CHESS_NUM BOARD_LENGTH*BOARD_WIDTH
#define PIECE_NUM 7

typedef int Color;
typedef float Score;
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

	static const Chess min		=(Chess)0b0000;
	static const Chess max		=(Chess)0b1111;
};

namespace chessChar{
	static const char empty			='-';
	static const char dark			='X';
	namespace red{
		static const char king		='K';
		static const char guard		='G';
		static const char elephant	='M';
		static const char rook		='R';
		static const char knight	='N';
		static const char cannon	='C';
		static const char pawn		='P';
	};
	namespace black{
		static const char king		='k';
		static const char guard		='g';
		static const char elephant	='m';
		static const char rook		='r';
		static const char knight	='n';
		static const char cannon	='c';
		static const char pawn		='p';
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
	static const int death=33;
	static const int covered=34;
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
	static const int flip=2;
};

namespace material_value{
	static const Score king_value		= (Score)6 ;
	static const Score guard_value		= (Score)5 ;
	static const Score elephant_value	= (Score)4 ;
	static const Score rook_value		= (Score)3 ;
	static const Score knight_value		= (Score)2 ;
	static const Score cannon_value		= (Score)4 ;
	static const Score pawn_value		= (Score)1 ;
	static const Score empty_value		= (Score)0 ;
	static const Score dark_value		= (Score)0 ;

	static const Score pawn_with_king		= (Score)100 ;
	static const Score cannon_without_king	= (Score)100 ;
};

inline Color flipColor(Color colorIn){
#ifdef _WARNING_MESSAGE_
	if(colorIn!=chessColor::red&&colorIn!=chessColor::black)
		std::cout<<"Warning:void flipColor(Color)>>input color is unknown..."<<std::endl;
#endif
	return (Color)(((int)colorIn)^0b0001);
}

inline Color color(Chess chessIn){
	return (Color)(((int)chessIn)>>3);
}

inline Chess chessType(Chess chessIn){
	return (Chess)(((int)chessIn)&0b0111);
}
#endif