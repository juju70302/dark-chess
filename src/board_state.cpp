#include "board_state.h"

void BoardState::init(char bdIn[],int closedChessIn[],int myFirstIn,Color myColorIn){
	movableNum[0]=0;movableNum[1]=0;closedNum[0]=0;closedNum[1]=0;
	for(int i=0;i<2;i++)for(int j=0;j<7;j++)movableList[(i<<3)|j]=0;
	for(int i=0;i<CHESS_NUM;i++){
		board[i]=(Chess)charToInt(bdIn[i]);
		if(isMovable(board[i])){
			int clr=(int)colorWithoutCheck(board[i]);

			pieceList[(clr<<4)|movableNum[clr]].where=i;
			pieceList[(clr<<4)|movableNum[clr]].chess=board[i];
			pieceListIndex[i]=movableNum[clr];
			movableList[board[i]]++;
			movableNum[clr]++;
		}
		else
			pieceListIndex[i]=chessPos::illegal;
	}
	for(int i=0;i<2;i++){
		for(int j=0;j<PIECE_NUM;j++){
			closedChess[(i<<3)|j]=closedChessIn[i*PIECE_NUM+j];
			closedNum[i]+=closedChess[(i<<3)|j];
		}
	}
	setColor(myColorIn);
	setMyTurn((myFirstIn==0)?0:1);

#ifdef _USING_TRANS_TABLE_
	if((myColor()==chessColor::red)||(myColor()==chessColor::black))
		rehash();
#endif
}

int BoardState::isValid()const{
	for(int i=0;i<CHESS_NUM;i++)if(board[i]>15)return 0;

	int pieceCount[2*8],closedNumTmp=0;
	for(int i=0;i<2;i++){
		for(int j=0;j<PIECE_NUM;j++){
			pieceCount[(i<<3)|j]=closedChess[(i<<3)|j];
			closedNumTmp+=closedChess[(i<<3)|j];
		}
	}
	for(int i=0;i<CHESS_NUM;i++){
		if(isMovable(board[i]))
			pieceCount[(int)board[i]]++;
		else if(board[i]==chessNum::dark)closedNumTmp--;
	}
	if(closedNumTmp!=0)return 0;
	for(int i=0;i<2;i++){
		if(pieceCount[(i<<3)|chessNum::red::king]>1)return 0;
		if(pieceCount[(i<<3)|chessNum::red::pawn]>5)return 0;
		for(int j=(int)chessNum::red::guard;j<(int)chessNum::red::pawn;j++)
			if(pieceCount[(i<<3)|j]>2)return 0;
	}
	return 1;
}

void BoardState::printPieceList()const{
	std::cout<<"movableList( red )>>";
	for(int i=0;i<7;i++)std::cout<<" "<<movableList[i];
	std::cout<<std::endl<<"movableList(black)>>";
	for(int i=8;i<15;i++)std::cout<<" "<<movableList[i];
	std::cout<<std::endl<<"pieceList(red)>>   ";
	for(int i=0;i<movableNum[(int)chessColor::red];i++)
		std::cout<<intToChar(pieceList[((int)chessColor::red)<<4|i].chess)<<" ";
	std::cout<<std::endl<<"pieceList(black)>> ";
	for(int i=0;i<movableNum[(int)chessColor::black];i++)
		std::cout<<intToChar(pieceList[((int)chessColor::black)<<4|i].chess)<<" ";
	std::cout<<std::endl<<"pieceListIndex>>"<<std::endl;
	for(int i=0;i<CHESS_NUM;i++){
		std::cout<<"  ["<<i<<"] = ";
		if(pieceListIndex[i]!=chessPos::illegal)
			std::cout<<pieceListIndex[i]<<"("<<
				intToChar(pieceList[colorWithoutCheck(board[i])<<4|(pieceListIndex[i])].chess)<<")";
		else
			std::cout<<"X   ";
		if(i%3==2)std::cout<<std::endl;
	}
	std::cout<<std::endl;
}

std::string MaterialValue::toString(){
	std::stringstream ss;ss.clear();ss.str("");
	ss<<"  red\t\tblack\n";
	ss<<"["<<chessChar::red::king<<"] = "<<value[chessNum::red::king]<<"\t\t";
	ss<<"["<<chessChar::black::king<<"] = "<<value[chessNum::black::king]<<std::endl;
	ss<<"["<<chessChar::red::guard<<"] = "<<value[chessNum::red::guard]<<"\t\t";
	ss<<"["<<chessChar::black::guard<<"] = "<<value[chessNum::black::guard]<<std::endl;
	ss<<"["<<chessChar::red::elephant<<"] = "<<value[chessNum::red::elephant]<<"\t\t";
	ss<<"["<<chessChar::black::elephant<<"] = "<<value[chessNum::black::elephant]<<std::endl;
	ss<<"["<<chessChar::red::rook<<"] = "<<value[chessNum::red::rook]<<"\t\t";
	ss<<"["<<chessChar::black::rook<<"] = "<<value[chessNum::black::rook]<<std::endl;
	ss<<"["<<chessChar::red::knight<<"] = "<<value[chessNum::red::knight]<<"\t\t";
	ss<<"["<<chessChar::black::knight<<"] = "<<value[chessNum::black::knight]<<std::endl;
	ss<<"["<<chessChar::red::cannon<<"] = "<<value[chessNum::red::cannon]<<"\t\t";
	ss<<"["<<chessChar::black::cannon<<"] = "<<value[chessNum::black::cannon]<<std::endl;
	ss<<"["<<chessChar::red::pawn<<"] = "<<value[chessNum::red::pawn]<<"\t\t";
	ss<<"["<<chessChar::black::pawn<<"] = "<<value[chessNum::black::pawn]<<std::endl;
	ss<<"["<<chessChar::empty<<"] = "<<value[chessNum::empty]<<"\t\t";
	ss<<"["<<chessChar::dark<<"] = "<<value[chessNum::dark]<<std::endl;
	return ss.str();
}

void BoardState::print()const{
	for(int i=0;i<2;i++){
		for(int j=0;j<BOARD_LENGTH;j++)
			std::cout<<intToChar((int)board[i*BOARD_LENGTH+j]);
		std::cout<<"\t";
		for(int j=0;j<PIECE_NUM;j++)std::cout<<" "<<(int)closedChess[i*8+j];
		std::cout<<std::endl;
	}
	for(int i=2;i<BOARD_WIDTH;i++){
		for(int j=0;j<BOARD_LENGTH;j++)
			std::cout<<intToChar((int)board[i*BOARD_LENGTH+j]);
		if(i==2)std::cout<<"\tred\tO: ";
		else std::cout<<"\tblack\tO: ";
		std::cout<<movableNum[i-2]<<"  X: "<<closedNum[i-2]<<std::endl;
	}
	std::cout<<"my color>> ";
	if(myColor()==chessColor::red)std::cout<<"r";
	else if(myColor()==chessColor::black)std::cout<<"b";
	else std::cout<<"?";
	std::cout<<"\twho's turn>> ";
	if(myTurn())std::cout<<"me";
	else std::cout<<"opponent";
	std::cout<<std::endl;
}

void BoardState::printBoard()const{
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
			std::cout<<"Warning:int BoardState::charToInt(char):unkown input \""<<
			intIn<<"\"..."<<std::endl;
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

const int BoardState::goNextPos[]={
//go up...
	chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,
	0,	1,	2,	3,	4,	5,	6,	7,
	8,	9,	10,	11,	12,	13,	14,	15,
	16,	17,	18,	19,	20,	21,	22,	23,
//go down...
	8,	9,	10,	11,	12,	13,	14,	15,
	16,	17,	18,	19,	20,	21,	22,	23,
	24,	25,	26,	27,	28,	29,	30,	31,
	chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,chessPos::illegal,
//go left...
	chessPos::illegal,	0,	1,	2,	3,	4,	5,	6,
	chessPos::illegal,	8,	9,	10,	11,	12,	13,	14,
	chessPos::illegal,	16,	17,	18,	19,	20,	21,	22,
	chessPos::illegal,	24,	25,	26,	27,	28,	29,	30,
//go right...
	1,	2,	3,	4,	5,	6,	7,	chessPos::illegal,
	9,	10,	11,	12,	13,	14,	15,	chessPos::illegal,
	17,	18,	19,	20,	21,	22,	23,	chessPos::illegal,
	25,	26,	27,	28,	29,	30,	31,	chessPos::illegal
};

void BoardState::countJumpTable(int validTable[]){
	//0:empty 1:red 2:dark 3:black
	//static const int empty=0;	static const int red=1;
	//static const int dark=2;	static const int black=3;
	static const int invalidPos=-1;

	//validTable[dir][currentPos][1th chess]...[8th]
	int ch[8],tmpPos,revC[8];

	//jump rightward...
	for(int pos=0;pos<32;pos++)for(ch[0]=0;ch[0]<4;ch[0]++)for(ch[1]=0;ch[1]<4;ch[1]++)
	for(ch[2]=0;ch[2]<4;ch[2]++)for(ch[3]=0;ch[3]<4;ch[3]++)for(ch[4]=0;ch[4]<4;ch[4]++)
	for(ch[5]=0;ch[5]<4;ch[5]++)for(ch[6]=0;ch[6]<4;ch[6]++)for(ch[7]=0;ch[7]<4;ch[7]++){
		tmpPos=BoardState::jumpRightPosition(pos%8,ch);
		if(tmpPos==invalidPos)
			tmpPos=chessPos::illegal;
		else
			tmpPos=pos-(pos%8)+tmpPos;
		validTable[(chessDirection::right<<21) | (pos<<16) | (ch[0]<<14) | (ch[1]<<12) |
			(ch[2]<<10) | (ch[3]<<8) | (ch[4]<<6) | (ch[5]<<4) | (ch[6]<<2) | ch[7]]=tmpPos;
	}

	//jump leftward...
	for(int pos=0;pos<32;pos++)for(ch[0]=0;ch[0]<4;ch[0]++)for(ch[1]=0;ch[1]<4;ch[1]++)
	for(ch[2]=0;ch[2]<4;ch[2]++)for(ch[3]=0;ch[3]<4;ch[3]++)for(ch[4]=0;ch[4]<4;ch[4]++)
	for(ch[5]=0;ch[5]<4;ch[5]++)for(ch[6]=0;ch[6]<4;ch[6]++)for(ch[7]=0;ch[7]<4;ch[7]++){
		for(int i=0;i<8;i++)revC[i]=ch[7-i];
		tmpPos=BoardState::jumpRightPosition(7-(pos%8),revC);
		if(tmpPos==invalidPos)
			tmpPos=chessPos::illegal;
		else
			tmpPos=pos-(pos%8)+(7-tmpPos);
		validTable[(chessDirection::left<<21) | (pos<<16) | (ch[0]<<14) | (ch[1]<<12) |
			(ch[2]<<10) | (ch[3]<<8) | (ch[4]<<6) | (ch[5]<<4) | (ch[6]<<2) | ch[7]]=tmpPos;
	}

	//jump downward...
	for(int pos=0;pos<32;pos++)for(ch[0]=0;ch[0]<4;ch[0]++)for(ch[1]=0;ch[1]<4;ch[1]++)
	for(ch[2]=0;ch[2]<4;ch[2]++)for(ch[3]=0;ch[3]<4;ch[3]++)for(ch[4]=0;ch[4]<4;ch[4]++){
		tmpPos=BoardState::jumpDownPosition((pos-pos%8)/8,ch);
		if(tmpPos==invalidPos)
			tmpPos=chessPos::illegal;
		else
			tmpPos=tmpPos*8+pos%8;
		for(ch[5]=0;ch[5]<4;ch[5]++)for(ch[6]=0;ch[6]<4;ch[6]++)for(ch[7]=0;ch[7]<4;ch[7]++){
			validTable[(chessDirection::down<<21) | (pos<<16) | (ch[0]<<14) | (ch[1]<<12) |
			(ch[2]<<10) | (ch[3]<<8) | (ch[4]<<6) | (ch[5]<<4) | (ch[6]<<2) | ch[7]]=tmpPos;
		}
	}

	//jump upward...
	for(int pos=0;pos<32;pos++)for(ch[0]=0;ch[0]<4;ch[0]++)for(ch[1]=0;ch[1]<4;ch[1]++)
	for(ch[2]=0;ch[2]<4;ch[2]++)for(ch[3]=0;ch[3]<4;ch[3]++)for(ch[4]=0;ch[4]<4;ch[4]++){
		for(int i=0;i<4;i++)revC[i]=ch[3-i];
		tmpPos=BoardState::jumpDownPosition(3-(pos-pos%8)/8,revC);
		if(tmpPos==invalidPos)
			tmpPos=chessPos::illegal;
		else
			tmpPos=(3-tmpPos)*8+pos%8;
		for(ch[5]=0;ch[5]<4;ch[5]++)for(ch[6]=0;ch[6]<4;ch[6]++)for(ch[7]=0;ch[7]<4;ch[7]++){
			validTable[(chessDirection::up<<21) | (pos<<16) | (ch[0]<<14) | (ch[1]<<12) |
			(ch[2]<<10) | (ch[3]<<8) | (ch[4]<<6) | (ch[5]<<4) | (ch[6]<<2) | ch[7]]=tmpPos;
		}
	}
}

int BoardState::jumpRightPosition(int currentPos,int pos[8]){
	//0:empty 1:red 2:dark 3:black
	//currentPos:0-7
	static const int empty=0;static const int red=1;
	static const int dark=2;static const int black=3;
	static const int illegalPosition=-1;
	int target;

	if(pos[currentPos]==empty||pos[currentPos]==dark)return illegalPosition;
	for(target=currentPos+1;target<8 && pos[target]==empty;target++);
	if(target>=8)return illegalPosition;
	for(target++;target<8 && pos[target]==empty;target++);
	if(target>=8)return illegalPosition;
	if(pos[target]==dark||pos[target]==empty)return illegalPosition;
	if((pos[currentPos]==red && pos[target]==red)||(pos[currentPos]==black && pos[target]==black))
		return illegalPosition;
	return target;
}

int BoardState::jumpDownPosition(int currentPos,int pos[4]){
	//0:empty 1:red 2:dark 3:black
	//currentPos:0-3
	static const int empty=0;static const int red=1;
	static const int dark=2;static const int black=3;
	static const int illegalPosition=-1;
	int target;

	if(pos[currentPos]==empty||pos[currentPos]==dark)return illegalPosition;
	for(target=currentPos+1;target<4 && pos[target]==empty;target++);
	if(target>=4)return illegalPosition;
	for(target++;target<4 && pos[target]==empty;target++);
	if(target>=4)return illegalPosition;
	if(pos[target]==dark||pos[target]==empty)return illegalPosition;
	if((pos[currentPos]==red && pos[target]==red)||(pos[currentPos]==black && pos[target]==black))
		return illegalPosition;
	return target;
}

void printEatTable(){
	int resultTable[16][16];
	for(int i=0;i<16;i++)for(int j=0;j<16;j++)resultTable[i][j]=4;
	for(int currentChess=0;currentChess<16;currentChess++){
		for(int nextChess=0;nextChess<16;nextChess++){
			if(currentChess==chessNum::empty||currentChess==chessNum::dark){
				resultTable[currentChess][nextChess]=0;
				continue;
			}
			if(nextChess==chessNum::empty){//next position is empty...
				resultTable[currentChess][nextChess]=1;
				continue;
			}
			if(nextChess==chessNum::dark){//next position is dark
				resultTable[currentChess][nextChess]=0;
				continue;
			}

			//remain eat condition...
			//Donnot eat same color...
			if(BoardState::colorWithoutCheck(currentChess)==BoardState::colorWithoutCheck(nextChess)){
				resultTable[currentChess][nextChess]=0;
				continue;
			}

			//Our chess is larger than enemy...
			//No pawn and cannon with us and no pawn with enemy...
			if(BoardState::type(currentChess)<chessNum::cannon&&BoardState::type(nextChess)<chessNum::pawn){
				if(BoardState::type(currentChess)<=BoardState::type(nextChess))
					resultTable[currentChess][nextChess]=1;
				else
					resultTable[currentChess][nextChess]=0;
				continue;
			}

			//cannon cannot eat anything without jump...
			if(BoardState::type(currentChess)==chessNum::cannon){
				resultTable[currentChess][nextChess]=0;
				continue;
			}

			//pawn can only eat king...
			if(BoardState::type(currentChess)==chessNum::pawn){
				if(BoardState::type(nextChess)==chessNum::king){
					resultTable[currentChess][nextChess]=1;
					continue;
				}
				resultTable[currentChess][nextChess]=0;
			}

			//enemy pawn can be eaten by any chess expcept our king...
			if(BoardState::type(nextChess)==chessNum::pawn){
				if(BoardState::type(currentChess)==chessNum::king)
					resultTable[currentChess][nextChess]=0;
				else
					resultTable[currentChess][nextChess]=1;
				continue;
			}
		}
	}
	for(int currentChess=0;currentChess<16;currentChess++){
		for(int nextChess=0;nextChess<16;nextChess++){
			std::cout<<resultTable[currentChess][nextChess];
			if(!(currentChess==16-1&&nextChess==16-1))std::cout<<",";
		}
		std::cout<<std::endl;
	}
}

int BoardState::jumpTable[4*32*4*4*4*4*4*4*4*4];