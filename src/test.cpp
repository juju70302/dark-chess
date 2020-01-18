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
void testAI();
void testBigChess();

int main(){
	init();
	testBigChess();
	//testAI();
	//testGenerate();
	//testTrabsTable();
	//testMove();
	//testEvaluate();

	return 0;
}

void testBigChess(){
	char bdIn[32]={
		'k','-','-','-','-','-','c','-',
		'P','N','-','-','n','-','-','-',
		'-','-','-','-','-','-','-','-',
		'-','-','-','-','-','-','-','-'
	};
	int closed[]={
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,0
	};

	AI ai;
	ai.state.init(bdIn,closed,0,chessColor::unknown);
	cout<<"original board>>"<<endl<<endl;ai.state.print();
	cout<<"piece list>>"<<endl;ai.state.printPieceList();

	//BigChess bc;
	//cout<<"flag1"<<endl;
	MaterialValue mv;
	//cout<<"flag2"<<endl;
	//cout<<" red  biggest = \""<<BoardState::intToChar(bc.big[(int)chessColor::red])<<"\""<<endl;
	//cout<<"black biggest = \""<<BoardState::intToChar(bc.big[(int)chessColor::black])<<"\""<<endl;
	mv.reset(ai.state.movableList,ai.state.closedChess);
	cout<<endl;
	cout<<" red  biggest = \""<<BoardState::intToChar(mv.big.big[(int)chessColor::red])<<"\""<<endl;
	cout<<"black biggest = \""<<BoardState::intToChar(mv.big.big[(int)chessColor::black])<<"\""<<endl;
	cout<<"material value>>"<<endl<<mv.toString();

	struct Move moveIn;
	moveIn.type=moveType::go;		moveIn.dir=chessDirection::up;
	moveIn.currentPos=8;			moveIn.nextPos=0;
	moveIn.currentChess=ai.state.board[moveIn.currentPos];
	moveIn.nextChess=ai.state.board[moveIn.nextPos];

	ai.state.moveWithoutCheck(moveIn);
	cout<<"after move board>>"<<endl<<endl;ai.state.print();

	mv.removeChessAfter(ai.state.movableList,ai.state.closedChess,moveIn.nextChess);
	cout<<endl;
	cout<<" red  biggest = \""<<BoardState::intToChar(mv.big.big[(int)chessColor::red])<<"\""<<endl;
	cout<<"black biggest = \""<<BoardState::intToChar(mv.big.big[(int)chessColor::black])<<"\""<<endl;
	cout<<"material value>>"<<endl<<mv.toString();
}

void testAI(){
	char bdIn[32]={
		'X','X','X','X','X','X','X','X',
		'X','X','X','X','X','X','X','X',
		'X','X','X','X','X','X','X','X',
		'X','X','X','X','X','X','X','X'
	};
	int closed[]={
		1,2,2,2,2,2,5,
		1,2,2,2,2,2,5
	};

	AI ai;
	ai.state.init(bdIn,closed,0,chessColor::unknown);
	cout<<"original board>>"<<endl<<endl;ai.state.print();

	struct Move firstMove,secondMove,thirdMove;
	ai.startGameMove(firstMove);
	firstMove.nextChess=chessNum::red::knight;
	if(ai.move(firstMove))cout<<"first move >>\t"<<firstMove.toString()<<endl;
	else cout<<"Error to first move >>\t"<<firstMove.toString()<<endl;

	cout<<endl<<"myTurn1=\""<<ai.state.myTurn()<<endl<<endl;

	cout<<endl<<"board>>"<<endl;ai.state.print();cout<<endl;

	ai.startGameMove(secondMove);
	secondMove.nextChess=chessNum::black::pawn;
	if(ai.move(secondMove))cout<<"second move >>\t"<<secondMove.toString()<<endl;
	else cout<<"Error to second move >>\t"<<secondMove.toString()<<endl;

	cout<<endl<<"board>>"<<endl;ai.state.print();cout<<endl;

	cout<<endl<<"myTurn2=\""<<ai.state.myTurn()<<endl<<endl;


	thirdMove.type=moveType::go;thirdMove.currentPos=3;thirdMove.nextPos=11;
	thirdMove.currentChess=ai.state.board[thirdMove.currentPos];
	thirdMove.nextChess=ai.state.board[thirdMove.nextPos];
	thirdMove.dir=chessDirection::down;
	//ai.state.moveWithoutCheck(thirdMove);
	//ai.state.flipTurn();
	//ai.state.flipTurn();
///*
	cout<<endl<<"board>>"<<endl;ai.state.print();cout<<endl;

	//static const Score infMaxScore=(Score)10000000;
	//negaScout2(ai.state,ai.transTable,-1*infMaxScore,infMaxScore,3);
	//ai.state.flipTurn();
	//ai.state.unMove(thirdMove);

	//ai.completeSearchMove(thirdMove,1500);
	//ai.noFlipSearchMove(thirdMove,1500);
	cout<<"move num=\""<<noFlipMoveNum(ai.state)<<"\""<<endl;

	ai.moveGenerator(thirdMove);

	cout<<"third move >>\t"<<thirdMove.toString()<<endl;
	char m0,m1;int pos1=31;ai.positionChange(m0,m1,pos1);
	cout<<"pos \""<<pos1<<"\" into \""<<m0<<m1<<"\""<<endl;
	m0='b',m1='1';
	ai.positionChange(pos1,m0,m1);
	cout<<"char \""<<m0<<m1<<"\" into pos \""<<pos1<<"\""<<endl;
	//cout<<"choose \""<<ai.chooseFlip()<<"\" to flip...\n";
//*/
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
	//TransTable<struct TransData> tt;

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
		'-','-','-','c','-','-','-','-',
		'-','k','X','K','g','p','-','-',
		'-','-','X','X','-','-','-','-',
		'-','-','-','c','-','-','-','-'
	};
	int closed[]={
		0,0,0,0,0,0,0,
		0,0,0,0,0,0,3
	};


	AI ai;
	ai.state.init(bdIn,closed,1,chessColor::red);
	cout<<"original board>>"<<endl<<endl;ai.state.print();

	cout<<endl<<"pieceList>>"<<endl;ai.state.printPieceList();

	struct Move mList[2000];
	int order[2000];
	int mNum=generateMove(ai.state,mList,chessColor::red,order);
	cout<<endl<<"move num=\""<<mNum<<"\""<<endl;
	//for(int i=0;i<mNum;i++)cout<<mList[order[i]].toString()<<endl;cout<<endl;

#ifdef _USING_TRANS_TABLE_
	//int point=negaScout(ai.state,ai.transTable,(Score)-100,(Score)100.0,15);

	//float point=chanceNodeSearch(ai.state,ai.transTable,10,(Score)-100,(Score)100,3);
	float point=negaScout1(ai.state,ai.transTable,-1*10000,10000,6);
#else
	//int point=negaScout(ai.state,-100,100,15);
#endif
	cout<<"count point=\""<<point<<"\""<<endl<<endl;
	cout<<"choose \""<<ai.chooseFlip()<<"\" to flip...\n";

	//Chess redL,blackL;
	//ai.state.largestChess(redL,blackL);
	//cout<<"red largest = \""<<BoardState::intToChar(redL)<<"\""<<endl;
	//cout<<"black largest = \""<<BoardState::intToChar(blackL)<<"\""<<endl;

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

	//cout<<endl<<"pieceList>>"<<endl;g.printPieceList();

	g.flipChessWithoutCheck(1,chessNum::black::guard);
	cout<<endl<<"after flip>>"<<endl;g.print();
	//cout<<"pieceList>>"<<endl;g.printPieceList();
/*
	g.unMove(1,1,chessNum::dark);
	cout<<endl<<"after unmove>>"<<endl;g.print();
	cout<<"pieceList>>"<<endl;g.printPieceList();
//*/
///*
	Move m;
	m.currentPos=9;m.dir=chessDirection::down;
	m.type=moveType::flip;
	m.nextChess=chessNum::black::guard;
	cout<<endl;
	if(g.canMove(m.dir,m.currentPos,m.type,m.nextPos,m.nextChess))
		cout<<"can move\""<<BoardState::intToChar(g.board[m.currentPos])<<"\""<<endl;
	else
		cout<<"cannot move..."<<endl;
	//g.goWithoutCheck(currentDirIn,currentPosIn);
	//g.jumpWithoutCheck(m.dir,m.currentPos,m.nextPos,m.nextChess);
	g.moveWithoutCheck(m);
	cout<<endl<<"after move>>"<<endl;g.print();
	//cout<<"pieceList>>"<<endl;g.printPieceList();
///*
	g.unMove(m);
	cout<<endl<<"after unmove>>"<<endl;g.print();
	//cout<<"pieceList>>"<<endl;g.printPieceList();
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
	//static const int invalidPos=-1;
	//int changeTable[16]={
		//1,	1,	1,	1,	1,	1,	1,	0,	3,	3,	3,	3,	3,	3,	3,	2
	//};
	int pos[]={black,red,red,empty,dark,empty,empty,empty};
	int chessPosIn=31;
	int chessDirIn=chessDirection::up;
	cout<<"jump to \""<<
	BoardState::jumpTable[(chessDirIn<<21)|(chessPosIn<<16)|(pos[0]<<14)|(pos[1]<<12)|
		(pos[2]<<10)|(pos[3]<<8)|(pos[4]<<6)|(pos[5]<<4)|(pos[6]<<2)|pos[7]]<<"\""<<endl;

}

void init(){
	BoardState::countJumpTable();
}