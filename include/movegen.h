#ifndef CHESS_ENGINE_MOVEGEN_H
#define CHESS_ENGINE_MOVEGEN_H

#include "position.h"
#include "movelist.h"

void movegen_generate_pseudo_legal(Position *pos, MoveList *list);

#endif