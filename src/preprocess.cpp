#include "trans_table.h"
#include <fstream>

int main(){
	std::string path="src\\";
	path+=HASH_VALUE_FILE;

	int r1=TransTable::writeHashValueDefineFile(path);
#ifdef _WARNING_MESSAGE_
	if(r1==1)std::cout<<"success to writing file \""<<path<<"\""<<std::endl;
	else if(r1==-1) std::cout<<"fail to write..."<<std::endl<<"hash collision!!!"<<std::endl;
	else std::cout<<"fail to write..."<<std::endl;
#endif
	return 0;
}