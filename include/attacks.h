#ifndef CHESS_ENGINE_ATTACKS_H
#define CHESS_ENGINE_ATTACKS_H

#include "position.h"

void attacks_init(void);

Bitboard attacks_pawn(Color side, Square from);
Bitboard attacks_knight(Square from);
Bitboard attacks_king(Square from);

Bitboard attacks_bishop(Square from, Bitboard occupied);
Bitboard attacks_rook(Square from, Bitboard occupied);
Bitboard attacks_queen(Square from, Bitboard occupied);

Bitboard position_attackers_to(const Position *pos, Square square);

int position_is_square_attacked(const Position *pos, Square square, Color attacker);
int position_is_in_check(const Position *pos, Color side);

#endif