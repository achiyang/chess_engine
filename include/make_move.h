#ifndef CHESS_ENGINE_MAKE_MOVE_H
#define CHESS_ENGINE_MAKE_MOVE_H

#include "position.h"
#include "move.h"

typedef struct PositionUndo {
    Piece captured_piece;

    CastlingRights castling_rights;
    Square en_passant;

    int halfmove_clock;
    int fullmove_number;
} PositionUndo;

void position_make_move(Position *pos, Move move, PositionUndo *undo);

void position_unmake_move(Position *pos, Move move, const PositionUndo *undo);

#endif