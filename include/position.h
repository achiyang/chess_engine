#ifndef CHESS_ENGINE_POSITION_H
#define CHESS_ENGINE_POSITION_H

#include "types.h"

typedef struct Position {
    Piece           board[SQUARE_NB];
    Bitboard        pieces[PIECE_NB];
    Bitboard        occupancies[COLOR_NB];

    Color           side_to_move;
    CastlingRights  castling_rights;
    Square          en_passant;

    int             halfmove_clock;
    int             fullmove_number;
} Position;

typedef uint32_t PositionStatus;

enum PositionStatusFlag {
    POSITION_STATUS_OK                     = 0,

    POSITION_ERROR_NULL_POSITION           = 1U << 0,

    POSITION_ERROR_INVALID_SIDE_TO_MOVE    = 1U << 1,
    POSITION_ERROR_INVALID_CASTLING_RIGHTS = 1U << 2,
    POSITION_ERROR_INVALID_EN_PASSANT      = 1U << 3,

    POSITION_ERROR_INVALID_HALFMOVE_CLOCK  = 1U << 4,
    POSITION_ERROR_INVALID_FULLMOVE_NUMBER = 1U << 5,

    POSITION_ERROR_INVALID_PIECE           = 1U << 6,
    POSITION_ERROR_PIECES_OVERLAP          = 1U << 7,
    POSITION_ERROR_PIECES_MISMATCH         = 1U << 8,
    POSITION_ERROR_OCCUPANCY_OVERLAP       = 1U << 9,
    POSITION_ERROR_OCCUPANCY_MISMATCH      = 1U << 10,

    POSITION_ERROR_WHITE_KING_COUNT        = 1U << 11,
    POSITION_ERROR_BLACK_KING_COUNT        = 1U << 12
};

/* 
 * 이 모듈의 조작 함수들은 Precondition을 검사하지 않는다.
 * caller는 valid input과 Position invariant를 보장해야 한다.
 */

/*
 * Precondition:
 * - pos != NULL
 * 
 * Effect:
 * - pos를 empty position으로 초기화한다.
 * 
 * - board[] = { NO_PIECE, ... }
 * - pieces[] = { 0 }
 * - occupancies[] = { 0 }
 * - side_to_move = WHITE
 * - castling_rights = CASTLING_NONE
 * - en_passant = NO_SQUARE
 * - halfmove_clock = 0
 * - fullmove_number = 1
 */
void position_clear(Position *pos);

/*
 * Precondition:
 * - pos != NULL
 * - piece_is_valid(piece)
 * - square_is_valid(square)
 * - pos->board[square] == NO_PIECE
 * 
 * Effect:
 * - square에 piece를 배치한다.
 * - board[], pieces[], occupancies[]를 함께 갱신한다.
 */
void position_set_piece(Position *pos, Piece piece, Square square);

/*
 * Precondition:
 * - pos != NULL
 * - square_is_valid(square)
 * - pos->board[square] != NO_PIECE
 * 
 * Effect:
 * - square에 있는 piece를 제거한다.
 * - board[], pieces[], occupancies[]를 함께 갱신한다.
 * 
 * Return:
 * - 제거된 piece를 반환한다.
 */
Piece position_remove_piece(Position *pos, Square square);

/*
 * Precondition:
 * - pos != NULL
 * - square_is_valid(from)
 * - square_is_valid(to)
 * - from != to
 * - pos->board[from] != NO_PIECE
 * - pos->board[to] == NO_PIECE
 * 
 * Effect:
 * - from square의 piece를 to square로 이동한다.
 * - board[], pieces[], occupancies[]를 함께 갱신한다.
 * - capture는 처리하지 않는다.
 */
void position_move_piece(Position *pos, Square from, Square to);

PositionStatus position_validate(const Position *pos);

#endif