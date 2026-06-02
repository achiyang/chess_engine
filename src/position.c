#include "position.h"

#include <string.h>

void position_clear(Position *pos) {
    for (Square square = A1; square < SQUARE_NB; ++square) {
        pos->board[square] = NO_PIECE;
    }

    memset(pos->pieces, 0, sizeof(pos->pieces));
    memset(pos->occupancies, 0, sizeof(pos->occupancies));

    pos->side_to_move = WHITE;
    pos->castling_rights = CASTLING_NONE;
    pos->en_passant = NO_SQUARE;

    pos->halfmove_clock = 0;
    pos->fullmove_number = 1;
}

void position_set_piece(Position *pos, Piece piece, Square square) {
    Color color = piece_color_of(piece);
    Bitboard bb = square_bb(square);

    pos->board[square] = piece;
    pos->pieces[piece] |= bb;
    pos->occupancies[color] |= bb;
    pos->occupancies[BOTH] |= bb;
}

Piece position_remove_piece(Position *pos, Square square) {
    Piece piece = pos->board[square];
    Color color = piece_color_of(piece);
    Bitboard bb = square_bb(square);

    pos->board[square] = NO_PIECE;
    pos->pieces[piece] &= ~bb;
    pos->occupancies[color] &= ~bb;
    pos->occupancies[BOTH] &= ~bb;

    return piece;
}

void position_move_piece(Position *pos, Square from, Square to) {
    Piece piece = pos->board[from];
    Color color = piece_color_of(piece);
    Bitboard move_bb = square_bb(from) | square_bb(to);

    pos->board[from] = NO_PIECE;
    pos->board[to] = piece;

    pos->pieces[piece] ^= move_bb;
    pos->occupancies[color] ^= move_bb;
    pos->occupancies[BOTH] ^= move_bb;
}