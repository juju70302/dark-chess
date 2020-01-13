#include "MyAI.h"
#include "board_state.h"
#include "search.h"
#include <iostream>
#include <cstdio>

using namespace std;

void init();
void testJumpTable();
void testMove();
void testEvaluate();
void testGenerate();

int main(){
	init();
	testGenerate();
	//testMove();
	//testEvaluate();

	return 0;
}

void testGenerate(){
	char bdIn[32]={
		'-','-','-','-','-','-','-','-',
		'K','-','-','-','-','-','-','-',
		'-','-','-','-','-','-','-','-',
		'k','-','P','-','-','-','-','-'
	};
	int closed[]={
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0
	};

	BoardState g;
	g.init(bdIn,closed,0,chessColor::red);
	cout<<"original board>>"<<endl<<endl;g.print();

	//cout<<endl<<"pieceList>>"<<endl;g.printPieceList();

	struct Move mList[200];
	int mNum=generateMove(g,mList,chessColor::black);
	cout<<endl<<"move num=\""<<mNum<<"\""<<endl;
	for(int i=0;i<mNum;i++){
		cout<<mList[i].toString()<<endl;
	}
	cout<<endl;

	int point=negaScout(g,-100,100,2);
	cout<<"count point=\""<<point<<"\""<<endl<<endl;

	//cout<<"after movement's board>>"<<endl<<endl;g.print();
	//cout<<endl<<"pieceList>>"<<endl;g.printPieceList();

	//cout<<"flip red=\""<<BoardState::flipColor(chessColor::red)<<"\""<<endl;
	//cout<<"flip red twice=\""<<BoardState::flipColor(BoardState::flipColor(chessColor::red))<<"\""<<endl;
	//cout<<"flip black=\""<<BoardState::flipColor(chessColor::black)<<"\""<<endl;
	//cout<<"flip black twice=\""<<BoardState::flipColor(BoardState::flipColor(chessColor::black))<<"\""<<endl;

	//BoardState g1;g1.init(bdIn,closed,1,chessColor::black);
	//cout<<"turn=\""<<g1.myTurn<<"\""<<endl;
	//g1.flipTurn();
	//cout<<"flip turn=\""<<g1.myTurn<<"\""<<endl;
	//g1.flipTurn();
	//cout<<"flip turn twice=\""<<g1.myTurn<<"\""<<endl;
}

void testEvaluate(){
	char bdIn[32]={
		'-','X','-','c','C','C','X','X',
		'-','X','X','X','G','X','X','X',
		'-','X','X','X','X','c','X','-',
		'X','X','m','X','X','m','G','X'
	};
	int closed[]={
		0,0,2,2,2,0,3,
		0,2,0,2,2,0,4
	};

	BoardState g;
	g.init(bdIn,closed,1,chessColor::black);

	cout<<"original board>>"<<endl;
	cout<<endl;g.print();

	evaluatingFunction(g);
	cout<<"pieceList>>"<<endl;g.printPieceList();

}

void testMove(){
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
	BoardState g;
	g.init(bdIn,closed,1,chessColor::unknown);

	cout<<"original board>>"<<endl;g.print();cout<<endl;
	if(g.isValid())cout<<"board is valid..."<<endl;
	else cout<<"board illegal!!!"<<endl;

	cout<<endl<<"pieceList>>"<<endl;g.printPieceList();

	g.flipChessWithoutCheck(1,chessNum::black::guard);
	cout<<endl<<"after flip>>"<<endl;g.print();
	cout<<"pieceList>>"<<endl;g.printPieceList();
/*
	g.unMove(1,1,chessNum::dark);
	cout<<endl<<"after unmove>>"<<endl;g.print();
	cout<<"pieceList>>"<<endl;g.printPieceList();
//*/
///*
	Move m;
	m.currentPos=0;m.dir=chessDirection::down;
	m.type=moveType::go;
	cout<<endl;
	if(g.canMove(m.dir,m.currentPos,m.type,m.nextPos,m.nextChess))
		cout<<"can move\""<<BoardState::intToChar(g.board[m.currentPos])<<"\""<<endl;
	else
		cout<<"cannot move..."<<endl;
	//g.goWithoutCheck(currentDirIn,currentPosIn);
	//g.jumpWithoutCheck(m.dir,m.currentPos,m.nextPos,m.nextChess);
	g.moveWithoutCheck(m);
	cout<<endl<<"after move>>"<<endl;g.print();
	cout<<"pieceList>>"<<endl;g.printPieceList();
///*
	g.unMove(m);
	cout<<endl<<"after unmove>>"<<endl;g.print();
	cout<<"pieceList>>"<<endl;g.printPieceList();
/*
	g.unMove(1,1,chessNum::dark);
	cout<<endl<<"after unmove>>"<<endl;g.print();
	cout<<"pieceList>>"<<endl;g.printPieceList();
//*/
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