#include "definition.h"
//#include "board_state.h"
#include <functional>
#include <fstream>

#ifndef _TRANS_TABLE_H_
#define _TRANS_TABLE_H_

typedef uint64_t KeyType;
#define HASH_VALUE_FILE "hash_value.cpp"

struct TransData{
	Score score;
	int8_t depth;
	uint8_t flag;
	static const uint8_t no		= 0b0000;
	static const uint8_t exact	= 0b0001;
	static const uint8_t upper	= 0b0010;
	static const uint8_t lower	= 0b0011;
	inline struct TransData& operator=(const struct TransData& dataIn){
		score=dataIn.score;
		depth=dataIn.depth;
		flag=dataIn.flag;
	};
};

template<class DataType>class HashTable{
public:
	static const int MAX_TABLE_SIZE_BIT=20;
	struct Value{
		KeyType key;
		DataType data;
	}*table;

	const int sizeBit;
	const KeyType modMask;

	inline HashTable(int tableSizeBitIn=MAX_TABLE_SIZE_BIT);
	inline ~HashTable();

	inline void init(struct Value& valueIn)const;
	inline void clear();

	inline int isHit(KeyType keyIn,DataType** dataOut);
	inline void update(KeyType keyIn,const DataType& dataIn);
};

template<class DataType>
inline void HashTable<DataType>::update(KeyType keyIn,const DataType& dataIn){
	table[keyIn&modMask].data=dataIn;
	table[keyIn&modMask].key=keyIn;
}


template<class DataType>
inline int HashTable<DataType>::isHit(KeyType keyIn,DataType** dataOut){
	if(table[keyIn&modMask].key!=keyIn)return 0;
	(*dataOut)=&(table[keyIn&modMask].data);
	return 1;
}

template<class DataType>
inline HashTable<DataType>::HashTable(int tableSizeBitIn):table(nullptr),sizeBit(tableSizeBitIn),
  modMask(~((~0)<<tableSizeBitIn)){
	table=new struct Value[(1<<tableSizeBitIn)];
	clear();
}

template<class DataType>
inline HashTable<DataType>::~HashTable(){
	delete table;
}

template<class DataType>
inline void HashTable<DataType>::init(struct Value& valueIn)const{
	valueIn.data.flag=valueIn.data.no;
}

template<class DataType>
inline void HashTable<DataType>::clear(){
	for(int i=0;i<(1<<sizeBit);i++)init(table[i]);
}

class TransTable:public HashTable<struct TransData>{
public:
	static const KeyType chessPosToHash[16*64];
	static const KeyType colorToHash[2];

	static inline void countHashValue(KeyType chessPosH[],KeyType colorH[]);
	static inline int writeHashValueDefineFile(const std::string& pathIn);

	inline TransTable(int tableSizeBitIn=HashTable::MAX_TABLE_SIZE_BIT);
	inline ~TransTable();

	static inline KeyType hash(int posIn,Chess chessIn);
	static inline KeyType hash(Color colorIn);
	static inline KeyType hash(const Chess boardIn[]);
	static inline KeyType hash(KeyType keyA,KeyType keyB);

	static inline KeyType flipColor(KeyType keyIn);
	static inline KeyType removeChess(KeyType keyIn,int posIn,Chess chessIn);
	static inline KeyType addChess(KeyType keyIn,int posIn,Chess chessIn);
};

inline KeyType TransTable::addChess(KeyType keyIn,int posIn,Chess chessIn){
	return hash(keyIn,hash(posIn,chessIn));
}

inline KeyType TransTable::removeChess(KeyType keyIn,int posIn,Chess chessIn){
	return hash(keyIn,hash(posIn,chessIn));
}

inline KeyType TransTable::flipColor(KeyType keyIn){
	return hash(keyIn,hash(colorToHash[chessColor::red],colorToHash[chessColor::black]));
}

inline KeyType TransTable::hash(KeyType keyA,KeyType keyB){
	return keyA^keyB;
}

inline KeyType TransTable::hash(const Chess boardIn[]){
	KeyType res=0;
	for(int i=0;i<32;i++)
		if(boardIn[i]!=chessNum::empty)
			res=hash(res,hash(i,boardIn[i]));
	return res;
}

inline KeyType TransTable::hash(Color colorIn){
	return colorToHash[(int)colorIn];
}

inline KeyType TransTable::hash(int posIn,Chess chessIn){
	return chessPosToHash[(posIn<<5)|(int)chessIn];
}

inline TransTable::TransTable(int tableSizeBitIn):HashTable(tableSizeBitIn){

}

inline TransTable::~TransTable(){
}

inline void TransTable::countHashValue(KeyType chessPosH[],KeyType colorH[]){
	std::hash<std::string>hashGenerator;
	for(int i=0;i<16*64;i++)
		chessPosH[i]=(KeyType)hashGenerator(std::to_string(i));
	for(int i=0;i<2;i++)
		colorH[i]=(KeyType)hashGenerator(std::to_string(16*64+i));
}

inline int TransTable::writeHashValueDefineFile(const std::string& pathIn){
	std::string strWrt="#ifndef _HashValueDefineFile_\n";
	strWrt+="#define _HashValueDefineFile_\n";
	strWrt+="#include \"definition.h\"\n#include \"trans_table.h\"\n";

	KeyType chessPosHash[16*64];
	KeyType colorHash[2];
	countHashValue(chessPosHash,colorHash);

	for(int i=0;i<16*64;i++){
		for(int j=0;j<16*64;j++){
			if(i==j)continue;
			if(chessPosHash[i]==chessPosHash[j])
				return -1;
		}
	}
	for(int i=0;i<2;i++){
		for(int j=0;j<2;j++){
			if(i==j)continue;
			if(colorHash[i]==colorHash[j])
				return -1;
		}
	}
	for(int i=0;i<16*64;i++)
		for(int j=0;j<2;j++)
			if(chessPosHash[i]==colorHash[j])
				return -1;

	strWrt+="const KeyType TransTable::chessPosToHash[]={\n";
	for(int i=0;i<16;i++){
		for(int j=0;j<64;j++){
			strWrt+=std::to_string(chessPosHash[i*64+j]);
			strWrt+="U";
			if(i!=15||j!=63)strWrt+=", ";
		}
		strWrt+="\n";
	}
	strWrt+="};\n\n";

	strWrt+="const KeyType TransTable::colorToHash[]={\n";
	for(int i=0;i<2;i++){
		strWrt+=std::to_string(colorHash[i]);
		strWrt+="U";
		if(i!=1)strWrt+=", ";
	}

	strWrt+="};\n#endif\n";

	std::ofstream out_file;
	out_file.open(pathIn);

	if(!out_file.is_open()){
#ifdef _WARNING_MESSAGE_
		std::cout<<"Error:TransTable::writeHashValueDefineFile(const string)>>Cannot open file\""<<pathIn<<"\""<<std::endl;
#endif
		return 0;
	}

	out_file<<strWrt;

	out_file.close();

	return 1;
}

#ifdef _PRE_PROCESS_
const KeyType TransTable::chessPosToHash[16*64]={0};
const KeyType TransTable::colorToHash[2]={0};
#endif

#endif