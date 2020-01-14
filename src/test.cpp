#include "MyAI.h"
#include "board_state.h"
#include "search.h"
#include "AI.h"
#include <iostream>
#include <cstdio>

using namespace std;

void init();
void testJumpTable();
void testMove();
void testEvaluate();
void testGenerate();
void testTrabsTable();

int main(){
	init();
	testGenerate();
	//testTrabsTable();
	//testMove();
	//testEvaluate();

	return 0;
}

void testTrabsTable(){
#ifdef _USING_TRANS_TABLE_
	char bdIn1[32]={
		'-','-','-','-','-','-','-','-',
		'k','X','C','-','-','-','-','-',
		'-','-','-','-','-','-','-','-',
		'-','-','-','-','-','-','-','-'
	};
	int closed1[]={
		0,1,0,0,0,0,0,
		0,0,0,0,0,0,0
	};
	AI ai1;
	TransTable tt;

	ai1.state.init(bdIn1,closed1,1,chessColor::red);


	char bdIn2[32]={
		'-','-','-','-','-','-','-','-',
		'C','X','-','-','-','-','-','-',
		'-','-','-','-','-','-','-','-',
		'-','-','-','-','-','-','-','-'
	};
	int closed2[]={
		0,1,0,0,0,0,0,
		0,0,0,0,0,0,0
	};
	AI ai2;ai2.state.init(bdIn2,closed2,1,chessColor::red);

	KeyType hValue3=ai1.state.hashValue;
	
	cout<<"ai1's hash\t=\t\""<<ai1.state.hashValue<<"\""<<endl;
	cout<<"ai2's hash\t=\t\""<<ai2.state.hashValue<<"\""<<endl;

	//KeyType hValue1=TransTable::hash(1,chessNum::red::king);
	//hValue1=TransTable::hash(hValue1,TransTable::hash(9,chessNum::black::pawn));
	//cout<<"original hash1\t=\t\""<<hValue1<<"\""<<endl;
	//KeyType hValue2=TransTable::hash(0,chessNum::red::king);
	//hValue2=TransTable::hash(hValue2,TransTable::hash(9,chessNum::black::pawn));
	//cout<<"original hash2\t=\t\""<<hValue2<<"\""<<endl;
	//hValue2=TransTable::hash(hValue2,TransTable::hash(0,chessNum::red::king));
	//cout<<"remove hash2\t=\t\""<<hValue2<<"\""<<endl;
	//hValue2=TransTable::hash(hValue2,TransTable::hash(1,chessNum::red::king));
	//cout<<"add hash2\t=\t\""<<hValue2<<"\""<<endl;
	//hValue2=0;
	//hValue2=TransTable::hash(hValue2,hValue1);
	//cout<<"original hash1\t=\t\""<<hValue1<<"\""<<endl;
	//cout<<"original hash2\t=\t\""<<hValue2<<"\""<<endl;

	cout<<"game1>>"<<endl;ai1.state.print();
	cout<<endl<<"game2>>"<<endl;ai2.state.print();

	//tt.update(ai1.state.hashValue,12,TransTable::Value::Data::exact,10);
	//TransTable::Value::Data* dataOut;
	//if((int)tt.isHit(ai1.state.hashValue,&dataOut))cout<<"value1 hit,get value \""<<dataOut->score<<"\""<<endl;
	//else cout<<"no value1 in table..."<<endl;
	//if((int)tt.isHit(ai2.state.hashValue,&dataOut))cout<<"value hit,get value \""<<dataOut->score<<"\""<<endl;
	//else cout<<"no value in table..."<<endl;

	struct Move m1;
	m1.type=moveType::flip;
	m1.currentPos=9;
	m1.dir=chessDirection::up;
	m1.nextChess=chessNum::red::guard;
	ai1.state.canMove(m1);
	//ai1.state.moveWithoutCheck(m1);
	ai1.state.flipTurn();
	cout<<endl<<"game1(after move)>>"<<endl;ai1.state.print();cout<<endl;
	cout<<"ai1's hash\t=\t\""<<ai1.state.hashValue<<"\""<<endl;
	cout<<"ai2's hash\t=\t\""<<ai2.state.hashValue<<"\""<<endl;
	//ai1.state.unMove(m1);
	ai1.state.flipTurn();
	cout<<"ai1's original\t=\t\""<<hValue3<<"\""<<endl;
	cout<<"ai1's unmove\t=\t\""<<ai1.state.hashValue<<"\""<<endl;
#endif
}

void testGenerate(){
	char bdIn[32]={
		'-','-','-','-','-','-','-','-',
		'K','-','-','-','-','-','-','-',
		'-','-','-','-','-','-','-','-',
		'g','-','P','-','-','-','-','-'
	};
	int closed[]={
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0
	};

	AI ai;
	ai.state.init(bdIn,closed,1,chessColor::red);
	cout<<"original board>>"<<endl<<endl;ai.state.print();

	//cout<<endl<<"pieceList>>"<<endl;g.printPieceList();

	struct Move mList[200];
	int mNum=generateMove(ai.state,mList,chessColor::red);
	cout<<endl<<"move num=\""<<mNum<<"\""<<endl;
	for(int i=0;i<mNum;i++)cout<<mList[i].toString()<<endl;cout<<endl;

#ifdef _USING_TRANS_TABLE_
	int point=negaScout(ai.state,ai.transTable,-100,100,20);
#else
	int point=negaScout(ai.state,-100,100,15);
#endif
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