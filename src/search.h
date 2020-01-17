#include "board_state.h"
#include "trans_table.h"
#include <algorithm>

#ifndef _SEARCH_H_
#define _SEARCH_H_

#define negaScout negaScout2

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

	static const Score pawn_with_king		= (Score)5 ;
	static const Score cannon_without_king	= (Score)5 ;
};

//------------------------------predeclared funcion----------------------------------------
inline Score evaluatingFunction(const BoardState& boardIn);
inline Score materialValues(const BoardState& boardIn);
inline Score openMaterialValues(const BoardState& boardIn,const Score materialValueTable[16]);
inline Score closedMaterialValues(const BoardState& boardIn,const Score materialValueTable[16]);
inline int generateMoveWithoutFlip(BoardState& boardIn,Move moveList[],Color turn,int order[]);
inline int generateMove(BoardState& boardIn,Move moveList[],Color turn,int order[]);
inline int generateFlip(BoardState& boardIn,Move moveList[]);
inline int moveOrdering(const BoardState& boardIn,Move moveList[],int order[],int moveNum);
inline int noFlipMoveNum(const BoardState& boardIn,Color colorIn);

#ifdef _USING_TRANS_TABLE_
inline float chanceNodeSearch(BoardState& boardIn,TransTable& transTableIn,int posIn,Score alpha,Score beta,int depth);
#else
inline float chanceNodeSearch(BoardState& boardIn,int posIn,Score alpha,Score beta,int depth);
#endif

//------------------------------defining funcion----------------------------------------
inline int moveOrdering(const BoardState& boardIn,Move moveList[],int order[],int moveNum){
#ifdef _USING_MOVE_ORDERING_
	Chess ourLarge,emyLarge;Color clr=(boardIn.myTurn)?boardIn.myColor:boardIn.flipColor(boardIn.myColor);
	if(clr==chessColor::red)boardIn.largestChess(ourLarge,emyLarge);
	else boardIn.largestChess(emyLarge,ourLarge);

	int orderNum=0,recorded[Move::maxBranch+1];
	for(int i=0;i<moveNum;i++)recorded[i]=0;


	//run our largest chess...
	int needRunLarge=0;
	for(int i=0;i<moveNum&&needRunLarge==0;i++){
		if(moveList[i].currentChess==ourLarge){
			needRunLarge=boardIn.isDangerous(moveList[i].currentPos);
		}
	}
	if(needRunLarge){
		for(int i=0;i<moveNum;i++){
			if(moveList[i].currentChess==ourLarge){
				order[orderNum]=i;
				recorded[i]=1;
				orderNum++;
			}
		}
	}
	for(int i=0;i<moveNum;i++){//Eating enemy's king...
		if(recorded[i]||moveList[i].type==moveType::flip)continue;
		if(moveList[i].nextChess==emyLarge){
			order[orderNum++]=i;
			recorded[i]=1;
		}
	}

	for(int i=0;i<moveNum;i++){//Capturing being eaten...
		if(recorded[i]||moveList[i].type==moveType::flip)continue;
		if(boardIn.isDangerous(moveList[i].currentPos)){
			order[orderNum++]=i;
			recorded[i]=1;
		}
	}

	for(int i=0;i<moveNum;i++){//Eating enemy's chess...
		if(recorded[i]||moveList[i].type==moveType::flip)continue;
		if(boardIn.isMovable(moveList[i].nextChess)){
			order[orderNum++]=i;
			recorded[i]=1;
		}
	}

	for(int i=0;i<moveNum;i++){//ecept flip...
		if(recorded[i]||moveList[i].type==moveType::flip)continue;
		order[orderNum++]=i;
		recorded[i]=1;
	}

	for(int i=0;i<moveNum;i++){//remain flip...
		if(recorded[i])continue;
		order[orderNum++]=i;
		recorded[i]=1;
	}
	return 1;
#else
	for(int i=0;i<moveNum;i++)order[i]=i;
		return 1;
#endif
}

//with chance node search...
#ifdef _USING_TRANS_TABLE_
inline Score negaScout2(BoardState& boardIn,TransTable& transTableIn,Score alpha,Score beta,int depth)
#else
inline Score negaScout2(BoardState& boardIn,Score alpha,Score beta,int depth)
#endif
{
	//terminal consition
	if(depth==0||boardIn.isOver()){
		return evaluatingFunction(boardIn);
	}

	static const Score inf=(Score)1000000;
#ifdef _USING_TRANS_TABLE_
	struct TransData* hashValue;
	int hitResult;
#endif
	Score m,n,scoutT;
	struct Move moveList[Move::maxBranch+1];
	int moveNum,order[Move::maxBranch+1];

	m=-1*inf;
#ifdef _USING_TRANS_TABLE_
	//test whether hit the hash table...
	hashValue=nullptr;
	hitResult=transTableIn.isHit(boardIn.hashValue,&hashValue);
	if(hitResult){
		if(hashValue->depth >= depth){
			//Immediately return while hit exactly value!!
			if(hashValue->flag == TransData::exact)
				return hashValue->score;
			if(hashValue->flag == TransData::upper){
				alpha=std::max(alpha,hashValue->score);
				if(alpha>=beta)return alpha;
			}
			if(hashValue->flag == TransData::lower){
				beta=std::min(beta,hashValue->score);
				if(beta<=alpha)return alpha;
			}
		}
		else if(hashValue->flag == TransData::exact){
			m=hashValue->score;
		}
	}
#endif

	//generating move...
	moveNum=generateMove(boardIn,moveList,(boardIn.myTurn==0)?boardIn.flipColor(boardIn.myColor):boardIn.myColor,order);

	if(moveNum==0){
		return evaluatingFunction(boardIn);
	}

	m=std::max(((Score)-1)*inf,m);	//the current lower bound; fail soft
	n=beta;		//the current upper bound
	for(int moveIndex=0;moveIndex<moveNum;moveIndex++){
		if(moveList[order[moveIndex]].type==moveType::flip){//flip
#ifdef _USING_TRANS_TABLE_
			scoutT=chanceNodeSearch(boardIn,transTableIn,moveList[order[moveIndex]].currentPos,std::max(alpha,m),beta,depth);
#else
			scoutT=chanceNodeSearch(boardIn,moveList[order[moveIndex]].currentPos,std::max(alpha,m),beta,depth);
#endif
			if(scoutT>m){
				m=scoutT;
			}
		}
		else{

			boardIn.moveWithoutCheck(moveList[order[moveIndex]]);
			boardIn.flipTurn();

			//scout first
#ifdef _USING_TRANS_TABLE_
			scoutT=-1*negaScout2(boardIn,transTableIn,-1*n,-1*std::max(alpha,m),depth-1);
#else
			scoutT=-1*negaScout2(boardIn,-1*n,-1*std::max(alpha,m),depth-1);
#endif
			//reset to original position...
			boardIn.flipTurn();
			boardIn.unMove(moveList[order[moveIndex]]);
			if(scoutT>m){
				if((n == beta)||(depth < 3)||(scoutT >= beta))
					m=scoutT;
				else{//re-search...
					boardIn.moveWithoutCheck(moveList[order[moveIndex]]);
					boardIn.flipTurn();

#ifdef _USING_TRANS_TABLE_
					m=-1*negaScout2(boardIn,transTableIn,-1*beta,-1*scoutT,depth-1);
#else
					m=-1*negaScout2(boardIn,-1*beta,-1*scoutT,depth-1);
#endif

					//reset to original position...
					boardIn.flipTurn();
					boardIn.unMove(moveList[order[moveIndex]]);


				}
			}
		}

		//cut-off
		if(m>=beta){

#ifdef _USING_TRANS_TABLE_
			//update trans table...
			if((!hitResult)||(hashValue->depth < depth)){
				struct TransData dataIn;
				dataIn.score=m;
				dataIn.flag=TransData::lower;
				dataIn.depth=depth;
				transTableIn.update(boardIn.hashValue,dataIn);
			}
#endif
			return m;
		}
		//set up a null window
		n=std::max(alpha,m)+1;
	}

#ifdef _USING_TRANS_TABLE_
	if(m>alpha){
		if((!hitResult)||(hashValue->depth < depth)||((hashValue->depth == depth)&&
		  (hashValue->flag != TransData::exact))){
			struct TransData dataIn;
			dataIn.score=m;
			dataIn.flag=TransData::exact;dataIn.depth=depth;
			transTableIn.update(boardIn.hashValue,dataIn);
		}
	}
	else{
		if((!hitResult)||(hashValue->depth < depth)){
			struct TransData dataIn;
			dataIn.score=m;
			dataIn.flag=TransData::upper;
			dataIn.depth=depth;
			transTableIn.update(boardIn.hashValue,dataIn);
		}
	}
#endif

	return m;
}

//no chance node search...
#ifdef _USING_TRANS_TABLE_
inline Score negaScout1(BoardState& boardIn,TransTable& transTableIn,Score alpha,Score beta,int depth)
#else
inline Score negaScout1(BoardState& boardIn,Score alpha,Score beta,int depth)
#endif
{
	//terminal consition
	if(depth==0||boardIn.isOver()){
		return evaluatingFunction(boardIn);
	}

	static const Score inf=(Score)1000000;
#ifdef _USING_TRANS_TABLE_
	struct TransData* hashValue;
	int hitResult;
#endif
	Score m,n,scoutT;
	struct Move moveList[Move::maxBranch+1];
	int moveNum,order[Move::maxBranch+1];

	m=-1*inf;
#ifdef _USING_TRANS_TABLE_
	//test whether hit the hash table...
	hashValue=nullptr;
	hitResult=transTableIn.isHit(boardIn.hashValue,&hashValue);
	if(hitResult){
		if(hashValue->depth >= depth){
			//Immediately return while hit exactly value!!
			if(hashValue->flag == TransData::exact)
				return hashValue->score;
			if(hashValue->flag == TransData::upper){
				alpha=std::max(alpha,hashValue->score);
				if(alpha>=beta)return alpha;
			}
			if(hashValue->flag == TransData::lower){
				beta=std::min(beta,hashValue->score);
				if(beta<=alpha)return alpha;
			}
		}
		else if(hashValue->flag == TransData::exact){
			m=hashValue->score;
		}
	}
#endif

	//generating move...
	moveNum=generateMoveWithoutFlip(boardIn,moveList,(boardIn.myTurn==0)?boardIn.flipColor(boardIn.myColor):boardIn.myColor,order);

	if(moveNum==0){
		return evaluatingFunction(boardIn);
	}

	m=std::max(((Score)-1)*inf,m);	//the current lower bound; fail soft
	n=beta;		//the current upper bound
	for(int moveIndex=0;moveIndex<moveNum;moveIndex++){
		boardIn.moveWithoutCheck(moveList[order[moveIndex]]);
		boardIn.flipTurn();

		//scout first
#ifdef _USING_TRANS_TABLE_
		scoutT=-1*negaScout1(boardIn,transTableIn,-1*n,-1*std::max(alpha,m),depth-1);
#else
		scoutT=-1*negaScout1(boardIn,-1*n,-1*std::max(alpha,m),depth-1);
#endif
		if(scoutT>m){
			if((n == beta)||(depth < 3)||(scoutT >= beta))
				m=scoutT;
			else//re-search...
#ifdef _USING_TRANS_TABLE_
				m=-1*negaScout1(boardIn,transTableIn,-1*beta,-1*scoutT,depth-1);
#else
				m=-1*negaScout1(boardIn,-1*beta,-1*scoutT,depth-1);
#endif
		}

		//cut-off
		if(m>=beta){
			boardIn.flipTurn();
			boardIn.unMove(moveList[order[moveIndex]]);

#ifdef _USING_TRANS_TABLE_
			//update trans table...
			if((!hitResult)||(hashValue->depth < depth)){
				struct TransData dataIn;
				dataIn.score=m;
				dataIn.flag=TransData::lower;
				dataIn.depth=depth;
				transTableIn.update(boardIn.hashValue,dataIn);
			}
#endif

			return m;
		}

		//set up a null window
		n=std::max(alpha,m)+1;

		//reset to original position...
		boardIn.flipTurn();
		boardIn.unMove(moveList[order[moveIndex]]);
	}
#ifdef _USING_TRANS_TABLE_
	if(m>alpha){
		if((!hitResult)||(hashValue->depth < depth)||((hashValue->depth == depth)&&
		  (hashValue->flag != TransData::exact))){
			struct TransData dataIn;
			dataIn.score=m;
			dataIn.flag=TransData::exact;dataIn.depth=depth;
			transTableIn.update(boardIn.hashValue,dataIn);
		}
	}
	else{
		if((!hitResult)||(hashValue->depth < depth)){
			struct TransData dataIn;
			dataIn.score=m;
			dataIn.flag=TransData::upper;
			dataIn.depth=depth;
			transTableIn.update(boardIn.hashValue,dataIn);
		}
	}
#endif

	return m;
}

#ifdef _USING_TRANS_TABLE_
inline float chanceNodeSearch(BoardState& boardIn,TransTable& transTableIn,int posIn,Score alpha,Score beta,int depth)
#else
inline float chanceNodeSearch(BoardState& boardIn,int posIn,Score alpha,Score beta,int depth)
#endif
{
	float vSum=0.0,prob;
	struct Move m;m.type=moveType::flip;m.currentPos=posIn;m.nextPos=posIn;
	int totalClosed=0;
	Score thisScore;

	for(int i=0;i<2;i++)for(int j=0;j<PIECE_NUM;j++)totalClosed+=boardIn.closedChess[(i<<3)|j];
	for(int i=0;i<2;i++){
		for(int j=0;j<PIECE_NUM;j++){
			if(boardIn.closedChess[(i<<3)|j]==0)continue;
			prob=(((float)boardIn.closedChess[(i<<3)|j]) / ((float)totalClosed));

			boardIn.flipChessWithoutCheck(posIn,(i<<3)|j);
			boardIn.flipTurn();

#ifdef _USING_TRANS_TABLE_
			thisScore=-1.0*negaScout1(boardIn,transTableIn,alpha,beta,depth-1);
#else
			thisScore=-1.0*negaScout1(boardIn,alpha,beta,depth-1);
#endif
			boardIn.flipTurn();
			boardIn.unMove(m);
			vSum+=(prob*((float)thisScore));
		}
	}
	return vSum;
}

inline int noFlipMoveNum(const BoardState& boardIn){
	Color colorIn=(boardIn.myTurn)?boardIn.myColor:BoardState::flipColor(boardIn.myColor);
	int moveNum=0;
	int currentPos,nextPos;
	Chess nextChess;

	//walk or jump move...
	for(int i=0;i<boardIn.movableNum[(int)colorIn];i++){
		currentPos=boardIn.pieceList[((int)colorIn<<4)|i].where;
		//go condition...
		for(int dirChoose=chessDirection::min;dirChoose<=chessDirection::max;dirChoose++){
			currentPos=boardIn.pieceList[((int)colorIn<<4)|i].where;
			if(boardIn.canGo(dirChoose,currentPos,nextPos,nextChess)){
				moveNum++;
			}
		}

		//jump condition...
		if(BoardState::type(boardIn.pieceList[((int)colorIn<<4)|i].chess)==chessNum::cannon){
			for(int dirChoose=chessDirection::min;dirChoose<=chessDirection::max;dirChoose++){
				currentPos=boardIn.pieceList[((int)colorIn<<4)|i].where;
				if(boardIn.canJump(dirChoose,currentPos,nextPos,nextChess)){
					moveNum++;
				}
			}
		}
	}
	return moveNum;
}

inline int generateMove(BoardState& boardIn,Move moveList[],Color turn,int order[]){
#ifdef _WARNING_MESSAGE_
	if((turn!=chessColor::red)&&(turn!=chessColor::black)){
		std::cout<<"Warning:generateMoveWithoutFlip(BoardState&,BoardState::Move[],Color)>>input turn is unknown..."<<std::endl;
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
				moveList[moveNum].currentChess=boardIn.board[moveList[moveNum].currentPos];
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
					moveList[moveNum].currentChess=boardIn.board[moveList[moveNum].currentPos];
					moveNum++;
				}
			}
		}
	}

	//flip...
	for(int i=0;i<CHESS_NUM;i++){
		if(boardIn.board[i]==chessNum::dark){
			moveList[moveNum].type=moveType::flip;
			moveList[moveNum].currentPos=i;
			moveList[moveNum].nextPos=i;
			moveList[moveNum].currentChess=chessNum::dark;
			moveList[moveNum].dir=chessDirection::up;
			moveNum++;
		}
	}
	moveOrdering(boardIn,moveList,order,moveNum);
	return moveNum;
}

inline int generateMoveWithoutFlip(BoardState& boardIn,Move moveList[],Color turn,int order[]){
#ifdef _WARNING_MESSAGE_
	if((turn!=chessColor::red)&&(turn!=chessColor::black)){
		std::cout<<"Warning:generateMoveWithoutFlip(BoardState&,BoardState::Move[],Color)>>input turn is unknown..."<<std::endl;
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
				moveList[moveNum].currentChess=boardIn.board[moveList[moveNum].currentPos];
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
					moveList[moveNum].currentChess=boardIn.board[moveList[moveNum].currentPos];
					moveNum++;
				}
			}
		}
	}
	moveOrdering(boardIn,moveList,order,moveNum);
	return moveNum;
}

inline Score evaluatingFunction(const BoardState& boardIn){
	static const int c1=1;

	return ((boardIn.myTurn==0)?-1:1)*c1*materialValues(boardIn);
}

inline Score materialValues(const BoardState& boardIn){
	static Score materialValueTable[] = {material_value::king_value,material_value::guard_value,
		material_value::elephant_value,material_value::rook_value,material_value::knight_value,
		material_value::cannon_value,material_value::pawn_value,material_value::empty_value,
		material_value::king_value,material_value::guard_value,material_value::elephant_value,
		material_value::rook_value,material_value::knight_value,material_value::cannon_value,
		material_value::pawn_value,material_value::dark_value};

	static const Score c1=(Score)2;
	static const Score c2=(Score)1;

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
	return c1*openMaterialValues(boardIn,materialValueTable) +
		c2*closedMaterialValues(boardIn,materialValueTable);
}

inline Score closedMaterialValues(const BoardState& boardIn,const Score materialValueTable[16]){
	Color clr=boardIn.myColor;
	Score value;
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

inline Score openMaterialValues(const BoardState& boardIn,const Score materialValueTable[16]){
	Color clr=boardIn.myColor;
	Score value;
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