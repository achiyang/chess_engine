#ifndef CHESS_ENGINE_FEN_H
#define CHESS_ENGINE_FEN_H

#include "position.h"

#define STARTPOS_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

typedef enum FenResult {
    FEN_OK,
    FEN_ERROR_NULL,
    FEN_ERROR_BOARD,
    FEN_ERROR_SIDE,
    FEN_ERROR_CASTLING,
    FEN_ERROR_EN_PASSANT,
    FEN_ERROR_HALFMOVE,
    FEN_ERROR_FULLMOVE,
    FEN_ERROR_TRAILING
} FenResult;

FenResult fen_parse(Position *pos, const char *fen);

#endif