#ifndef CHESS_ENGINE_POSITION_DEBUG_H
#define CHESS_ENGINE_POSITION_DEBUG_H

#include "types.h"
#include "position.h"

#include <stdio.h>

void position_print(const Position *pos, FILE *out);

#endif