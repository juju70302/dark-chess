#include "MyAI.h"
#include "board_state.h"
#include <iostream>
#include <cstdio>

using namespace std;

int main(){
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
	BoardState g;
	g.init(bdIn,closed,chessColor::unknown,chessColor::unknown);
	cout<<"result>>"<<endl;
	for(int i=0;i<4;i++){
		for(int j=0;j<8;j++){
			if(g.board[i*8+j]<=9)cout<<" ";
			cout<<(int)g.board[i*8+j]<<" ";
		}
		cout<<endl;
	}

	cout<<"convert>>"<<endl;
	g.print();
	printf("turn>>%d\n",g.myColor);
	printf("flip>>%d\n",g.flipColor(g.myColor));
	printf("flip>>%d\n",g.flipColor(g.flipColor(g.myColor)));
	cout<<(int)chessColor::unknown<<endl;
	cout<<(int)g.flipColor((int)chessColor::unknown)<<endl;
	cout<<(int)g.flipColor((int)g.flipColor((int)chessColor::unknown))<<endl;
}