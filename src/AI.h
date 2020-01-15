#include "definition.h"
#include "search.h"
#include "board_state.h"

#ifndef _AI_H_
#define _AI_H_
class AI{
public:
	BoardState state;
	TransTable transTable;

public:
	AI(int tableSizeBitIn=20);
};
#endif