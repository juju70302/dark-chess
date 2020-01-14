#include "board_state.h"
#include "trans_table.h"
#include <algorithm>

#ifndef _SEARCH_H_
#define _SEARCH_H_

namespace material_value{
	static const int king_value		= 6 ;
	static const int guard_value	= 5 ;
	static const int elephant_value	= 4 ;
	static const int rook_value		= 3 ;
	static const int knight_value	= 2 ;
	static const int cannon_value	= 4 ;
	static const int pawn_value		= 1 ;
	static const int empty_value	= 0 ;
	static const int dark_value		= 0 ;

	static const int pawn_with_king			= 5 ;
	static const int cannon_without_king	= 5 ;
};

//inline void scout();
inline int evaluatingFunction(const BoardState& boardIn);
inline int materialValues(const BoardState& boardIn);
inline int openMaterialValues(const BoardState& boardIn,const int materialValueTable[16]);
inline int closedMaterialValues(const BoardState& boardIn,const int materialValueTable[16]);
inline int generateMove(const BoardState& boardIn,Move moveList[],Color turn);

#ifdef _USING_TRANS_TABLE_
inline int negaScout(BoardState& boardIn,TransTable& transTableIn,int alpha,int beta,int depth){
	//terminal consition
	if(depth==0||boardIn.isOver())
		return evaluatingFunction(boardIn);

	static const int inf=1000000;
	TransTable::Value::Data* hashValue;int hitResult;
	int m,n,scoutT;
	struct Move moveList[Move::maxBranch+1];
	int moveNum;

	//test whether hit the hash table...
	hashValue=nullptr;m=-1*inf;
	hitResult=transTableIn.isHit(boardIn.hashValue,&hashValue);
	if(hitResult){
		if(hashValue->depth >= depth){
			//Immediately return while hit exactly value!!
			if(hashValue->flag == TransTable::Value::Data::exact)
				return hashValue->score;
			if(hashValue->flag == TransTable::Value::Data::upper){
				alpha=std::max(alpha,(int)hashValue->score);
				if(alpha>=beta)return alpha;
			}
			if(hashValue->flag == TransTable::Value::Data::lower){
				beta=std::min(beta,(int)hashValue->score);
				if(beta<=alpha)return alpha;
			}
		}
		else if(hashValue->flag == TransTable::Value::Data::exact){
			m=hashValue->score;
		}
	}

	//generating move...
	moveNum=generateMove(boardIn,moveList,(boardIn.myTurn==0)?boardIn.flipColor(boardIn.myColor):boardIn.myColor);

	if(moveNum==0){
//std::cout<<"Error:no move in moveList..."<<std::endl<<"board>>"<<std::endl;boardIn.print();std::cout<<std::endl;
		return evaluatingFunction(boardIn);
	}

	m=std::max(-1*inf,m);	//the current lower bound; fail soft
	n=beta;		//the current upper bound
	for(int moveIndex=0;moveIndex<moveNum;moveIndex++){
		boardIn.moveWithoutCheck(moveList[moveIndex]);
		boardIn.flipTurn();

		//scout first
		scoutT=-1*negaScout(boardIn,transTableIn,-1*n,-1*std::max(alpha,m),depth-1);

//std::cout<<std::endl;for(int i=0;i<(2-depth);i++)std::cout<<"  ";
//std::cout<<std::endl<<"score=\""<<scoutT<<"\""<<std::endl;
//std::cout<<"board>>"<<std::endl;boardIn.print();

		if(scoutT>m){
			if((n == beta)||(depth < 3)||(scoutT >= beta))
				m=scoutT;
			else//re-search...
				m=-1*negaScout(boardIn,transTableIn,-1*beta,-1*scoutT,depth-1);
		}

		//cut-off
		if(m>=beta){
			boardIn.flipTurn();
			boardIn.unMove(moveList[moveIndex]);

			//update trans table...
			if((!hitResult)||(hashValue->depth < depth))
				transTableIn.update(boardIn.hashValue,m,TransTable::Value::Data::lower,depth);

			return m;
		}

		//set up a null window
		n=std::max(alpha,m)+1;

		//reset to original position...
		boardIn.flipTurn();
		boardIn.unMove(moveList[moveIndex]);
	}
	if(m>alpha){
		if((!hitResult)||(hashValue->depth < depth)||((hashValue->depth == depth)&&
		  (hashValue->flag != TransTable::Value::Data::exact)))
			transTableIn.update(boardIn.hashValue,m,TransTable::Value::Data::exact,depth);
	}
	else{
		if((!hitResult)||(hashValue->depth < depth))
			transTableIn.update(boardIn.hashValue,m,TransTable::Value::Data::upper,depth);
	}

	return m;
}
#else
inline int negaScout(BoardState& boardIn,int alpha,int beta,int depth){
	//terminal consition
	if(depth==0||boardIn.isOver())	return evaluatingFunction(boardIn);

	static const int inf=1000000;
	int m,n,scoutT;
	struct Move moveList[Move::maxBranch+1];
	int moveNum;
	moveNum=generateMove(boardIn,moveList,(boardIn.myTurn==0)?boardIn.flipColor(boardIn.myColor):boardIn.myColor);

	if(moveNum==0){
//std::cout<<"Error:no move in moveList..."<<std::endl<<"board>>"<<std::endl;boardIn.print();std::cout<<std::endl;
		return evaluatingFunction(boardIn);
	}

	m=-1*inf;	//the current lower bound; fail soft
	n=beta;		//the current upper bound
	for(int moveIndex=0;moveIndex<moveNum;moveIndex++){
		boardIn.moveWithoutCheck(moveList[moveIndex]);
		boardIn.flipTurn();

		//scout first
		scoutT=-1*negaScout(boardIn,-1*n,-1*std::max(alpha,m),depth-1);

//std::cout<<std::endl;for(int i=0;i<(2-depth);i++)std::cout<<"  ";
//std::cout<<"score=\""<<scoutT<<"\""<<std::endl;
//std::cout<<"board>>"<<std::endl;boardIn.print();

		if(scoutT>m){
			if((n == beta)||(depth < 3)||(scoutT >= beta))
				m=scoutT;
			else//re-search...
				m=-1*negaScout(boardIn,-1*beta,-1*scoutT,depth-1);
		}

		//cut-off
		if(m>=beta){
			boardIn.flipTurn();
			boardIn.unMove(moveList[moveIndex]);
			return m;
		}

		//set up a null window
		n=std::max(alpha,m)+1;

		//reset to original position...
		boardIn.flipTurn();
		boardIn.unMove(moveList[moveIndex]);
	}
	return m;
}
#endif

inline int generateMove(const BoardState& boardIn,Move moveList[],Color turn){
#ifdef _WARNING_MESSAGE_
	if((turn!=chessColor::red)&&(turn!=chessColor::black)){
		std::cout<<"Warning:generateMove(BoardState&,BoardState::Move[],Color)>>input turn is unknown..."<<std::endl;
		std::cout<<"turn=\""<<turn<<"\""<<std::endl;
	}
#endif

	int moveNum=0;
	//Move thisMove;

	//walk or jump move...
	for(int i=0;i<boardIn.movableNum[(int)turn];i++){
		//go condition...
		for(int dirChoose=chessDirection::min;dirChoose<=chessDirection::max;dirChoose++){
			moveList[moveNum].currentPos=boardIn.pieceList[((int)turn<<4)|i].where;
			if(boardIn.canGo(dirChoose,moveList[moveNum].currentPos,moveList[moveNum].nextPos,moveList[moveNum].nextChess)){
				moveList[moveNum].dir=dirChoose;
				moveList[moveNum].type=moveType::go;
				moveNum++;
			}
		}

		//jump condition...
		if(BoardState::type(boardIn.pieceList[((int)turn<<4)|i].chess)==chessNum::cannon){
			for(int dirChoose=chessDirection::min;dirChoose<=chessDirection::max;dirChoose++){
				moveList[moveNum].currentPos=boardIn.pieceList[((int)turn<<4)|i].where;
				if(boardIn.canJump(dirChoose,moveList[moveNum].currentPos,moveList[moveNum].nextPos,moveList[moveNum].nextChess)){
					moveList[moveNum].dir=dirChoose;
					moveList[moveNum].type=moveType::jump;
					moveNum++;
				}
			}
		}
	}
	return moveNum;
}

inline int evaluatingFunction(const BoardState& boardIn){
	static const int c1=1;
	//static const float c2=0.5;

	return ((boardIn.myTurn==0)?-1:1)*c1*materialValues(boardIn);
}

inline int materialValues(const BoardState& boardIn){
	static int materialValueTable[] = {material_value::king_value,material_value::guard_value,
		material_value::elephant_value,material_value::rook_value,material_value::knight_value,
		material_value::cannon_value,material_value::pawn_value,material_value::empty_value,
		material_value::king_value,material_value::guard_value,material_value::elephant_value,
		material_value::rook_value,material_value::knight_value,material_value::cannon_value,
		material_value::pawn_value,material_value::dark_value};

	static const int c1=2;
	static const int c2=1;

	//dynamically caculate material values
	//if red king alive,rising black pawn's value...
	//if red king dead,rising red cannon's value
	if(boardIn.movableList[(int)chessNum::red::king]+boardIn.closedChess[(int)chessNum::red::king])
		materialValueTable[(int)chessNum::black::pawn]=material_value::pawn_with_king;
	else
		materialValueTable[(int)chessNum::red::cannon]=material_value::cannon_without_king;

	//if black king alive,rising red pawn's value...
	//if black king dead,rising black cannon's value
	if(boardIn.movableList[(int)chessNum::black::king]+boardIn.closedChess[(int)chessNum::black::king])
		materialValueTable[(int)chessNum::red::pawn]=material_value::pawn_with_king;
	else
		materialValueTable[(int)chessNum::black::cannon]=material_value::cannon_without_king;


	//std::cout<<"point="<<c1*(float)openMaterialValues(boardIn,materialValueTable) +
		//c2*(float)closedMaterialValues(boardIn,materialValueTable)<<std::endl;
	//std::cout<<"open mv="<<openMaterialValues(boardIn,materialValueTable)<<std::endl;
	//std::cout<<"closed mv="<<closedMaterialValues(boardIn,materialValueTable)<<std::endl;

	return c1*openMaterialValues(boardIn,materialValueTable) +
		c2*closedMaterialValues(boardIn,materialValueTable);
}

inline int closedMaterialValues(const BoardState& boardIn,const int materialValueTable[16]){
	Color clr=boardIn.myColor;
	int value;
	if(clr==chessColor::unknown)return 0;

	value=0;
	if(clr==chessColor::red){
		for(int i=0;i<7;i++)
			value+=(boardIn.closedChess[i]*materialValueTable[i]);
		for(int i=8;i<16;i++)
			value-=(boardIn.closedChess[i]*materialValueTable[i]);
	}
	else{
		for(int i=0;i<7;i++)
			value-=(boardIn.closedChess[i]*materialValueTable[i]);
		for(int i=8;i<16;i++)
			value+=(boardIn.closedChess[i]*materialValueTable[i]);
	}

	return value;
}

inline int openMaterialValues(const BoardState& boardIn,const int materialValueTable[16]){
	Color clr=boardIn.myColor;
	int value;
	if(clr==chessColor::unknown)return 0;

	value=0;
	if(clr==chessColor::red){
		for(int i=0;i<boardIn.movableNum[(int)chessColor::red];i++)
			value+=materialValueTable[(int)boardIn.pieceList[i].chess];
		for(int i=0;i<boardIn.movableNum[(int)chessColor::black];i++)
			value-=materialValueTable[(int)boardIn.pieceList[0b10000|i].chess];
	}
	else{
		for(int i=0;i<boardIn.movableNum[(int)chessColor::red];i++)
			value-=materialValueTable[(int)boardIn.pieceList[i].chess];
		for(int i=0;i<boardIn.movableNum[(int)chessColor::black];i++)
			value+=materialValueTable[(int)boardIn.pieceList[0b10000|i].chess];
	}

	return value;
}
#endif