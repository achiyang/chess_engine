#ifndef CHESS_ATTACK_H
#define CHESS_ATTACK_H

#include "position.h"

void attack_init(void);

int position_is_square_attacked(const Position *pos, Square square, Color attacker);

#endif