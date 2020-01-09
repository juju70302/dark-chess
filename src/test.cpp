#include "MyAI.h"
#include "board_state.h"
#include <iostream>
#include <cstdio>

using namespace std;

void init();
void testJumpTable();

int main(){
	char bdIn[32]={
		'K','X','-','c','C','C','X','X',
		'k','X','X','X','G','X','X','X',
		'-','X','X','X','X','c','X','-',
		'X','X','m','X','X','m','G','X'
	};
	int closed[]={
		0,0,2,2,2,0,3,
		0,2,0,2,2,0,4
	};
	init();
	BoardState g;
	g.init(bdIn,closed,1,chessColor::unknown);

	cout<<"original board>>"<<endl;
	g.print();
	if(g.isValid())cout<<"board is valid..."<<endl;
	else cout<<"board illegal!!!"<<endl;

	g.flipChessWithoutCheck(1,chessNum::black::guard);
	cout<<"after flip>>"<<endl;
	g.print();
	//testJumpTable();
	int currentPosIn=0;	int currentDirIn=chessDirection::down;
	int currentTypeIn=moveType::go;
	if(g.canMove(currentDirIn,currentPosIn,currentTypeIn))
		cout<<"can move\""<<BoardState::intToChar(g.board[currentPosIn])<<"\""<<endl;
	else
		cout<<"cannot move..."<<endl;
	g.goWithoutCheck(currentDirIn,currentPosIn);
	cout<<"after move>>"<<endl;
	g.print();

	return 0;
}

void testJumpTable(){
	//0:empty 1:red 2:dark 3:black
	static const int empty=0;	static const int red=1;
	static const int dark=2;	static const int black=3;
	static const int invalidPos=-1;
	int changeTable[16]={
		1,	1,	1,	1,	1,	1,	1,	0,	3,	3,	3,	3,	3,	3,	3,	2
	};
	int pos[]={black,red,red,empty,empty,empty,empty,empty};
	int chessPosIn=31;
	int chessDirIn=chessDirection::up;
	cout<<"jump to \""<<
	BoardState::jumpTable[(chessDirIn<<21)|(chessPosIn<<16)|(pos[0]<<14)|(pos[1]<<12)|
		(pos[2]<<10)|(pos[3]<<8)|(pos[4]<<6)|(pos[5]<<4)|(pos[6]<<2)|pos[7]]<<"\""<<endl;

}

void init(){
	BoardState::countJumpTable();
}