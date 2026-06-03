#ifndef CHESS_ENGINE_NOTATION_H
#define CHESS_ENGINE_NOTATION_H

#include "types.h"

Piece notation_piece_from_char(char c);
char notation_piece_to_char(Piece piece);

Square notation_square_from_coord(const char *coord);
void notation_square_to_coord(Square square, char out[3]);

Color notation_side_from_char(char c);
char notation_side_to_char(Color side);

#endif