#include "board_state.h"

void BoardState::print(){
	for(int i=0;i<BOARD_WIDTH;i++){
		for(int j=0;j<BOARD_LENGTH;j++)
			std::cout<<intToChar((int)board[i*BOARD_LENGTH+j]);
		std::cout<<std::endl;
	}
}

char BoardState::intToChar(int intIn){
	switch(intIn){
		case chessNum::empty:
			return chessChar::empty;
		case chessNum::dark:
			return chessChar::dark;
		case chessNum::red::king:
			return chessChar::red::king;
		case chessNum::red::guard:
			return chessChar::red::guard;
		case chessNum::red::elephant:
			return chessChar::red::elephant;
		case chessNum::red::rook:
			return chessChar::red::rook;
		case chessNum::red::knight:
			return chessChar::red::knight;
		case chessNum::red::cannon:
			return chessChar::red::cannon;
		case chessNum::red::pawn:
			return chessChar::red::pawn;
		case chessNum::black::king:
			return chessChar::black::king;
		case chessNum::black::guard:
			return chessChar::black::guard;
		case chessNum::black::elephant:
			return chessChar::black::elephant;
		case chessNum::black::rook:
			return chessChar::black::rook;
		case chessNum::black::knight:
			return chessChar::black::knight;
		case chessNum::black::cannon:
			return chessChar::black::cannon;
		case chessNum::black::pawn:
			return chessChar::black::pawn;
		default:
#ifdef _WARNING_MESSAGE_
			std::cout<<"Warning:int BoardState::charToInt(char):unkown input..."<<std::endl;
#endif
			return chessNum::empty;
	}
}

int BoardState::charToInt(char chIn){
	switch(chIn){
		case chessChar::empty:
			return chessNum::empty;
		case chessChar::dark:
			return chessNum::dark;
		case chessChar::red::king:
			return chessNum::red::king;
		case chessChar::red::guard:
			return chessNum::red::guard;
		case chessChar::red::elephant:
			return chessNum::red::elephant;
		case chessChar::red::rook:
			return chessNum::red::rook;
		case chessChar::red::knight:
			return chessNum::red::knight;
		case chessChar::red::cannon:
			return chessNum::red::cannon;
		case chessChar::red::pawn:
			return chessNum::red::pawn;
		case chessChar::black::king:
			return chessNum::black::king;
		case chessChar::black::guard:
			return chessNum::black::guard;
		case chessChar::black::elephant:
			return chessNum::black::elephant;
		case chessChar::black::rook:
			return chessNum::black::rook;
		case chessChar::black::knight:
			return chessNum::black::knight;
		case chessChar::black::cannon:
			return chessNum::black::cannon;
		case chessChar::black::pawn:
			return chessNum::black::pawn;
		default:
#ifdef _WARNING_MESSAGE_
			std::cout<<"Warning:int BoardState::charToInt(char):unkown input..."<<std::endl;
#endif
			return chessNum::empty;
	}
}