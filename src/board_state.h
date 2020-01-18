#include "definition.h"
#include "trans_table.h"
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
		ss<<"pos("<<currentPos<<") ";
		if(type!=moveType::flip){
			ss<<"dir=\"";
			if(dir==chessDirection::up)ss<<"^";
			else if(dir==chessDirection::down)ss<<"v";
			else if(dir==chessDirection::left)ss<<"<";
			else if(dir==chessDirection::right)ss<<">";
			else ss<<"X";
			ss<<"\" ";
		}
		ss<<"type=\"";
		if(type==moveType::go)ss<<"go";
		else if(type==moveType::jump)ss<<"jump";
		else if(type==moveType::flip)ss<<"flip";
		else ss<<"????";
		ss<<"\"";
		return ss.str();
	};
	static const int maxBranch=12*4+16*3+4*2+4*4;
};

class BigChess{
public:
	Chess big[2];
	BigChess(){init();};
	void init(){big[(int)chessColor::red]=chessNum::red::king;big[(int)chessColor::black]=chessNum::black::king;};
	inline void reset(const int movableListIn[],const int closedChessIn[]);
	inline int addChessAfter(const int movableListIn[],const int closedChessIn[],Chess added);
	inline int removeChessAfter(const int movableListIn[],const int closedChessIn[],Chess eaten);
	inline int isBadPosition(const int movableListIn[],const int closedChessIn[],Color colorIn)const;
};

inline int isBadPosition(const int movableListIn[],const int closedChessIn[],Color colorIn)const{
	//if(chessType(big[colorIn])<chessNum::cannon){//Isn't cannon or pawn
		//return ;
	//}

	//Don't worry if having any cannon...
	if(movableListIn[((int)colorIn)<<3|chessNum::cannon]+closedChessIn[((int)colorIn)<<3|chessNum::cannon])
		return 0;

	//If enemy's king is alive,but yours dead...
	if((movableListIn[((int)flipColor(colorIn))<<3|chessNum::king]+closedChessIn[((int)flipColor(colorIn))<<3|chessNum::king])&&
		!(movableListIn[((int)colorIn)<<3|chessNum::king]+closedChessIn[((int)colorIn)<<3|chessNum::king])){
		//And you don't have any pawn,you're dammed!!
		if(!(movableListIn[((int)colorIn)<<3|chessNum::pawn]+closedChessIn[((int)colorIn)<<3|chessNum::pawn]))
			return 1;

	}
}

//input lists must have been refreshed...
inline int BigChess::addChessAfter(const int movableListIn[],const int closedChessIn[],Chess added){
	if((chessType(added) == chessNum::cannon)||(chessType(added)==chessNum::pawn)||(chessType(added)==chessNum::king)){
		reset(movableListIn,closedChessIn);
		return 1;
	}
	if(chessType(added)>=big[color(added)])return 0;
	reset(movableListIn,closedChessIn);
	return 1;
}

//input lists must have been refreshed...
inline int BigChess::removeChessAfter(const int movableListIn[],const int closedChessIn[],Chess eaten){
	if(eaten!=big[color(eaten)])return 0;
	if(movableListIn[eaten] + closedChessIn[eaten])return 0;
	reset(movableListIn,closedChessIn);
	return 1;
}

inline void BigChess::reset(const int movableListIn[],const int closedChessIn[]){
	for(big[chessColor::red]=chessNum::red::king;big[chessColor::red]<=chessNum::red::pawn;big[chessColor::red]++){
		if(closedChessIn[big[chessColor::red]]+movableListIn[big[chessColor::red]])break;
	}
	for(big[chessColor::black]=chessNum::black::king;big[chessColor::black]<=chessNum::black::pawn;big[chessColor::black]++){
		if(closedChessIn[big[chessColor::black]]+movableListIn[big[chessColor::black]])break;
	}
	if(big[chessColor::red] > (Chess)chessType(big[chessColor::black])){
		if((closedChessIn[chessNum::red::cannon]+movableListIn[chessNum::red::cannon]))
			big[chessColor::red]=chessNum::red::cannon;
		else if((big[chessColor::black]==chessNum::black::king)&&
			((closedChessIn[chessNum::red::pawn]+movableListIn[chessNum::red::pawn])))
			big[chessColor::red]=chessNum::red::pawn;
	}

	if(big[chessColor::red] < (Chess)chessType(big[chessColor::black])){
		if((closedChessIn[chessNum::black::cannon]+movableListIn[chessNum::black::cannon]))
			big[chessColor::black]=chessNum::black::cannon;
		else if((big[chessColor::red]==chessNum::red::king)&&
			((closedChessIn[chessNum::black::pawn]+movableListIn[chessNum::black::pawn])))
			big[chessColor::black]=chessNum::black::pawn;
	}
}

class MaterialValue{
public:
	struct colorMaterialValue{
		Score king;
		Score guard;
		Score elephant;
		Score rook;
		Score knight;
		Score pawn;
		Score cannon;
		Chess bigChess;
	}red,black;
	//Chess bigChess[2];
	BigChess big;
	Score value[CHESS_NUM];

	inline MaterialValue();
	inline void init();
	inline void initValue();
	inline void reset(const int movableListIn[],const int closedChessIn[]);
	inline void resetValue(const int movableListIn[],const int closedChessIn[]);
	inline void removeChessAfter(const int movableListIn[],const int closedChessIn[],Chess eaten);
	inline void addChessAfter(const int movableListIn[],const int closedChessIn[],Chess eaten);
	std::string toString();
};

inline void MaterialValue::addChessAfter(const int movableListIn[],const int closedChessIn[],Chess eaten){
	if(big.addChessAfter(movableListIn,closedChessIn,eaten))
		resetValue(movableListIn,closedChessIn);
}

inline void MaterialValue::removeChessAfter(const int movableListIn[],const int closedChessIn[],Chess eaten){
	if(big.removeChessAfter(movableListIn,closedChessIn,eaten))
		resetValue(movableListIn,closedChessIn);
}

inline void MaterialValue::initValue(){
	value[(int)chessNum::red::king]			=	material_value::king_value;
	value[(int)chessNum::red::guard]		=	material_value::guard_value;
	value[(int)chessNum::red::elephant]		=	material_value::elephant_value;
	value[(int)chessNum::red::rook]			=	material_value::rook_value;
	value[(int)chessNum::red::knight]		=	material_value::knight_value;
	value[(int)chessNum::red::cannon]		=	material_value::cannon_value;
	value[(int)chessNum::red::pawn]			=	material_value::pawn_with_king;
	value[(int)chessNum::black::king]		=	material_value::king_value;
	value[(int)chessNum::black::guard]		=	material_value::guard_value;
	value[(int)chessNum::black::elephant]	=	material_value::elephant_value;
	value[(int)chessNum::black::rook]		=	material_value::rook_value;
	value[(int)chessNum::black::knight]		=	material_value::knight_value;
	value[(int)chessNum::black::cannon]		=	material_value::cannon_value;
	value[(int)chessNum::black::pawn]		=	material_value::pawn_with_king;
	value[(int)chessNum::empty]				=	material_value::empty_value;
	value[(int)chessNum::dark]				=	material_value::dark_value;
}

inline void MaterialValue::init(){
	initValue();
	big.init();
}

inline void MaterialValue::reset(const int movableListIn[],const int closedChessIn[]){
	big.reset(movableListIn,closedChessIn);
	resetValue(movableListIn,closedChessIn);
}

inline void MaterialValue::resetValue(const int movableListIn[],const int closedChessIn[]){
	initValue();
	//if red king dead,decreasing black pawn's value...
	if(movableListIn[(int)chessNum::red::king]+closedChessIn[(int)chessNum::red::king]){
		value[(int)chessNum::black::pawn]=material_value::pawn_with_king;
	}
	else
		value[(int)chessNum::black::pawn]=material_value::pawn_value;

	//if red largest chess smaller than black,rising red cannon's value
	//if()
		//value[(int)chessNum::red::cannon]=material_value::cannon_without_king;

	//if black king alive,rising red pawn's value...
	//if black king dead,rising black cannon's value
	if(!(movableListIn[(int)chessNum::black::king]+closedChessIn[(int)chessNum::black::king])){
		value[(int)chessNum::red::pawn]=material_value::pawn_value;
		value[(int)chessNum::black::cannon]=material_value::cannon_without_king;
	}
}

inline MaterialValue::MaterialValue(){
	init();
}

class BoardState{
public:
	struct PieceList{
		Chess chess;
		int where;
		struct PieceList& operator=(const struct PieceList& pl){chess=pl.chess;where=pl.where;return *this;};
	};
private:
	Color _myColor;
public:
	struct PieceList pieceList[2*16];
	int pieceListIndex[CHESS_NUM];

	int _myTurn;
	Chess board[CHESS_NUM];
	int closedChess[2*8];
	int movableNum[2];
	int closedNum[2];
	int movableList[2*8];
	static const int goNextPos[4*32];
#ifdef _USING_TRANS_TABLE_
	KeyType hashValue;
#endif
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
	inline void setColor(Color colorIn);
	inline Color myColor()const;
	inline void setMyTurn(int myTurnIn);
	inline int myTurn()const;
	inline void flipTurn();

	inline int canMove(struct Move& moveIn)const;
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

	static inline Chess type(Chess chessIn);
	static inline Color colorWithoutCheck(Chess chessIn);
	static inline int isMovable(Chess chessIn);

	inline void largestChess(Chess& redLargest,Chess& blackLargest)const;
	inline int isDangerous(int posIn)const;
	inline int isAttackedByCannon(int posIn,Color enemy)const;
	inline int isNext(int pos1,int pos2,int& dir);

	inline BoardState();
	inline void rehash();

	friend void testJumpTable();
private:
	static int jumpRightPosition(int currentPos,int pos[8]);
	static int jumpDownPosition(int currentPos,int pos[4]);
	inline void changePieceList(int currentPos,int nextPos);
	inline void removeFromPieceList(int currentPos);
	inline void addToPieceList(int currentPos,int chessIn);
};

inline BoardState::BoardState():_myColor(chessColor::unknown){
}

inline int BoardState::isNext(int pos1,int pos2,int& dirOut){
	if(pos1==chessPos::illegal||pos2==chessPos::illegal)return 0;
	for(int dir=chessDirection::min;dir<=chessDirection::max;dir++){
		if(pos2==goNextPos[(dir<<5)|pos1]){
			dirOut=dir;
			return 1;
		}
	}
	return 0;
}

inline int BoardState::isAttackedByCannon(int posIn,Color enemy)const{
	int needRunLarge=0;
	for(int dir=chessDirection::min;dir<=chessDirection::max;dir++){//watch enemy's cannon...
		int pos=posIn;
		int notEmptyNum=0;
		for(pos=BoardState::goNextPos[(dir<<5)|pos];pos!=chessPos::illegal;){
			if(board[pos]!=chessNum::empty)notEmptyNum++;
			if(notEmptyNum>=2)break;
			pos=BoardState::goNextPos[(dir<<5)|pos];
		}
		if(pos==chessPos::illegal)continue;
		if(BoardState::type(board[pos])==chessNum::cannon&&
		  colorWithoutCheck(board[pos])==enemy){
			needRunLarge=1;
			break;
		}
		if(needRunLarge)return 1;
	}
	return 0;
}

inline int BoardState::isDangerous(int posIn)const{
	if(posIn==chessPos::illegal)return 0;
	if(!isMovable(board[posIn]))return 0;

	Color ourColor=colorWithoutCheck(board[posIn]);
	int needRunLarge=0;
	for(int dir=chessDirection::min;dir<=chessDirection::max;dir++){//watch enemy's cannon...
		int pos=posIn;
		int notEmptyNum=0;
		for(pos=BoardState::goNextPos[(dir<<5)|pos];pos!=chessPos::illegal;){
			if(board[pos]!=chessNum::empty)notEmptyNum++;
			if(notEmptyNum>=2)break;
			pos=BoardState::goNextPos[(dir<<5)|pos];
		}
		if(pos==chessPos::illegal)continue;
		if(BoardState::type(board[pos])==chessNum::cannon&&
		  colorWithoutCheck(board[pos])!=ourColor){
			needRunLarge=1;
			break;
		}
		if(needRunLarge)break;
	}
	for(int dir=chessDirection::min;dir<=chessDirection::max;dir++){
		int pos=BoardState::goNextPos[(dir<<5)|posIn];
		int a;Chess b;
		if(canGo(dir,pos,a,b)){
			needRunLarge=1;
			break;
		}
	}
	if(needRunLarge)return 1;
	return 0;
}

inline void BoardState::largestChess(Chess& redLargest,Chess& blackLargest)const{
	for(redLargest=chessNum::red::king;redLargest<=chessNum::red::pawn;redLargest++){
		if(closedChess[redLargest]+movableList[redLargest])break;
	}
	for(blackLargest=chessNum::black::king;blackLargest<=chessNum::black::pawn;blackLargest++){
		if(closedChess[blackLargest]+movableList[blackLargest])break;
	}
	if(redLargest > (Chess)type(blackLargest)){
		if((closedChess[chessNum::red::cannon]+movableList[chessNum::red::cannon]))
			redLargest=chessNum::red::cannon;
		else if((blackLargest==chessNum::black::king)&&((closedChess[chessNum::red::pawn]+movableList[chessNum::red::pawn])))
			redLargest=chessNum::red::pawn;
	}

	if(redLargest < (Chess)type(blackLargest)){
		if((closedChess[chessNum::black::cannon]+movableList[chessNum::black::cannon]))
			blackLargest=chessNum::black::cannon;
		else if((redLargest==chessNum::red::king)&&((closedChess[chessNum::black::pawn]+movableList[chessNum::black::pawn])))
			blackLargest=chessNum::black::pawn;
	}
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

inline int BoardState::canMove(struct Move& moveIn)const{
	if(moveIn.type==moveType::flip){
		moveIn.nextPos=moveIn.currentPos;
		moveIn.currentChess=chessNum::dark;
		return 1;
	}
	return canMove(moveIn.dir,moveIn.currentPos,moveIn.type,moveIn.nextPos,moveIn.nextChess);
}

inline int BoardState::canMove(int dir,int currentPos,int moveTypeIn,int& nextPos,Chess& nextChess)const{
//dir must between 0-3,moveType must between 0-1,
//currentPos must between chessPos::min to chessPos::max... 
	if(moveTypeIn==moveType::go)//go
		return canGo(dir,currentPos,nextPos,nextChess);
	else if(moveTypeIn==moveType::flip){//flip
		nextPos=currentPos;
		return (board[currentPos]==chessNum::dark)?1:0;
	}
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
		return BoardState::jumpTable[(dir<<21)|(currentPos<<16)|(ch[0]<<14)|(ch[1]<<12)|
			(ch[2]<<10)|(ch[3]<<8)|(ch[4]<<6)|(ch[5]<<4)|(ch[6]<<2)|ch[7]];
	}
	for(int i=0;i<4;i++)ch[i]=changeTable[(int)board[(i<<3)|(currentPos%8)]];
	return BoardState::jumpTable[(dir<<21)|(currentPos<<16)|(ch[0]<<14)|(ch[1]<<12)|
		(ch[2]<<10)|(ch[3]<<8)];
}

inline void BoardState::unMove(const struct Move& moveIn){
	unMove(moveIn.nextPos,moveIn.currentPos,moveIn.nextChess);
}

inline void BoardState::unMove(int currentPos,int prevPos,Chess prevChess){
	if(currentPos==prevPos){//unflip
#ifdef _USING_TRANS_TABLE_
		hashValue=TransTable::removeChess(hashValue,currentPos,board[currentPos]);
		hashValue=TransTable::addChess(hashValue,currentPos,chessNum::dark);
#endif
		removeFromPieceList(currentPos);
		closedChess[board[currentPos]]++;
		closedNum[colorWithoutCheck(board[currentPos])]++;
		movableList[board[currentPos]]--;
		board[currentPos]=chessNum::dark;
		return;
	}
#ifdef _USING_TRANS_TABLE_
	hashValue=TransTable::removeChess(hashValue,currentPos,board[currentPos]);
	hashValue=TransTable::addChess(hashValue,prevPos,board[currentPos]);
#endif
	
	changePieceList(currentPos,prevPos);
	board[prevPos]=board[currentPos];
	if(isMovable(prevChess)){
#ifdef _USING_TRANS_TABLE_
		hashValue=TransTable::addChess(hashValue,currentPos,prevChess);
#endif
		addToPieceList(currentPos,prevChess);
		movableList[prevChess]++;
	}
	board[currentPos]=prevChess;
}

inline void BoardState::moveWithoutCheck(const struct Move& moveIn){
	if(moveIn.type==moveType::go)
		goWithoutCheck(moveIn.dir,moveIn.currentPos);
	else if(moveIn.type==moveType::flip)
		flipChessWithoutCheck(moveIn.currentPos,moveIn.nextChess);
	else//jump
		jumpWithoutCheck(moveIn.dir,moveIn.currentPos);
}

inline void BoardState::goWithoutCheck(int dir,int currentPos){
	int nextPos=goNextPos[(dir<<5)|currentPos];
	int nextChess=board[nextPos];
	if(isMovable(nextChess)){
		removeFromPieceList(nextPos);
		movableList[nextChess]--;
#ifdef _USING_TRANS_TABLE_
		hashValue=TransTable::removeChess(hashValue,nextPos,nextChess);
#endif
	}
#ifdef _USING_TRANS_TABLE_
	hashValue=TransTable::removeChess(hashValue,currentPos,board[currentPos]);
	hashValue=TransTable::addChess(hashValue,nextPos,board[currentPos]);
#endif
	changePieceList(currentPos,nextPos);
	board[nextPos]=board[currentPos];
	board[currentPos]=chessNum::empty;
}

inline void BoardState::jumpWithoutCheck(int dir,int currentPos){
	int nextPos=jumpPosition(dir,currentPos);
	int nextChess=board[nextPos];

	removeFromPieceList(nextPos);
	changePieceList(currentPos,nextPos);
#ifdef _USING_TRANS_TABLE_
	hashValue=TransTable::removeChess(hashValue,nextPos,nextChess);
	hashValue=TransTable::removeChess(hashValue,currentPos,board[currentPos]);
	hashValue=TransTable::addChess(hashValue,nextPos,board[currentPos]);
#endif

	movableList[nextChess]--;
	board[nextPos]=board[currentPos];
	board[currentPos]=chessNum::empty;
}

inline void BoardState::flipChessWithoutCheck(int posIn,Chess appearChess){
#ifdef _USING_TRANS_TABLE_
	hashValue=TransTable::removeChess(hashValue,posIn,chessNum::dark);
	hashValue=TransTable::addChess(hashValue,posIn,appearChess);
#endif
	board[posIn]=appearChess;
	closedChess[appearChess]--;
	closedNum[colorWithoutCheck(appearChess)]--;
	movableList[appearChess]++;
	addToPieceList(posIn,appearChess);
}

inline int BoardState::isMovable(Chess chessIn){
	return ((((int)chessIn)&0b0111)==0b0111)?0:1;
}

inline Color BoardState::colorWithoutCheck(Chess chessIn){
	return color(chessIn);
}

inline Color BoardState::flipColor(Color colorIn){
	return flipColor(colorIn);
}

inline void BoardState::flipTurn(){
#ifdef _WARNING_MESSAGE_
	if(myTurn!=0&&myTurn!=1)std::cout<<"Warning:void BoardState::flipTurn()>>myTurn is unknown..."<<std::endl;
#endif
#ifdef _USING_TRANS_TABLE_
	hashValue=TransTable::flipColor(hashValue);
#endif
	_myTurn^=0b0001;
}

inline Chess BoardState::type(Chess chessIn){
	return chessType(chessIn);
}

inline int BoardState::isOver()const{
	if((movableNum[(int)chessColor::red]==0)&&(closedNum[(int)chessColor::red]==0))
		return 1;
	if((movableNum[(int)chessColor::black]==0)&&(closedNum[(int)chessColor::black]==0))
		return 1;
	return 0;
}

inline void BoardState::setColor(Color colorIn){
	_myColor=colorIn;
	rehash();
}

inline Color BoardState::myColor()const{
	return this->_myColor;
}

inline int BoardState::myTurn()const{
	return this->_myTurn;
}

inline void BoardState::setMyTurn(int myTurnIn){
	_myTurn=myTurnIn;
	rehash();
}

inline void BoardState::rehash(){
#ifdef _USING_TRANS_TABLE_
	for(int i=0;i<CHESS_NUM;i++)if(board[i]<chessNum::min||board[i]>chessNum::max)return;
	if(myColor()==chessColor::red||myColor()==chessColor::black)
		hashValue=TransTable::hash(TransTable::hash((myTurn())?myColor():flipColor(myColor())),
			TransTable::hash(board));
	else
		hashValue=TransTable::hash(board);
#endif
}

void printEatTable();
#endif