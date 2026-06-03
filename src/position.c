#include "position.h"

#include "bitboard.h"
#include "notation.h"

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

static int bitboard_array_has_overlap(const Bitboard *bbs, int count) {
    Bitboard seen = 0;

    for (int i = 0; i < count; ++i) {
        if (seen & bbs[i]) {
            return 1;
        }

        seen |= bbs[i];
    }

    return 0;
}

PositionStatus position_validate(const Position *pos) {
    PositionStatus status = POSITION_STATUS_OK;

    if (!pos) {
        return POSITION_ERROR_NULL_POSITION;
    }

    if (!side_is_valid(pos->side_to_move)) {
        status |= POSITION_ERROR_INVALID_SIDE_TO_MOVE;
    }

    if (!castling_rights_is_valid(pos->castling_rights)) {
        status |= POSITION_ERROR_INVALID_CASTLING_RIGHTS;
    }

    if (pos->en_passant != NO_SQUARE) {
        if (!square_is_valid(pos->en_passant)) {
            status |= POSITION_ERROR_INVALID_EN_PASSANT;
        }
        else if (pos->board[pos->en_passant] != NO_PIECE) {
            status |= POSITION_ERROR_INVALID_EN_PASSANT;
        }
        else if (side_is_valid(pos->side_to_move)) {
            Rank valid_rank =
                pos->side_to_move == WHITE ?
                    RANK_6 :
                    RANK_3;

            if (square_rank(pos->en_passant) != valid_rank) {
                status |= POSITION_ERROR_INVALID_EN_PASSANT;
            }
        }
    }

    if (pos->halfmove_clock < 0) {
        status |= POSITION_ERROR_INVALID_HALFMOVE_CLOCK;
    }

    if (pos->fullmove_number < 1) {
        status |= POSITION_ERROR_INVALID_FULLMOVE_NUMBER;
    }

    Bitboard expected_pieces[PIECE_NB] = { 0 };
    Bitboard expected_occupancies[COLOR_NB] = { 0 };

    for (Square square = A1; square < SQUARE_NB; ++square) {
        Piece piece = pos->board[square];

        if (!piece_is_valid(piece) && piece != NO_PIECE) {
            status |= POSITION_ERROR_INVALID_PIECE;
            return status;
        }

        if (piece == NO_PIECE) {
            continue;
        }

        Color color = piece_color_of(piece);
        Bitboard bb = square_bb(square);

        expected_pieces[piece] |= bb;
        expected_occupancies[color] |= bb;
    }

    expected_occupancies[BOTH] =
        expected_occupancies[WHITE] | expected_occupancies[BLACK];

    if (bitboard_array_has_overlap(pos->pieces, PIECE_NB)) {
        status |= POSITION_ERROR_PIECES_OVERLAP;
        status |= POSITION_ERROR_PIECES_MISMATCH;
    }
    else {
        for (Piece piece = WP; piece < PIECE_NB; ++piece) {
            if (pos->pieces[piece] != expected_pieces[piece]) {
                status |= POSITION_ERROR_PIECES_MISMATCH;
                break;
            }
        }
    }

    if (bitboard_array_has_overlap(pos->occupancies, SIDE_NB)) {
        status |= POSITION_ERROR_OCCUPANCY_OVERLAP;
        status |= POSITION_ERROR_OCCUPANCY_MISMATCH;
    }
    else {
        for (Color color = WHITE; color < SIDE_NB; ++color) {
            if (pos->occupancies[color] != expected_occupancies[color]) {
                status |= POSITION_ERROR_OCCUPANCY_MISMATCH;
            }
        }
    }

    if (pos->occupancies[BOTH] != expected_occupancies[BOTH]) {
        status |= POSITION_ERROR_OCCUPANCY_MISMATCH;
    }

    if (bitboard_popcount(expected_pieces[WK]) != 1) {
        status |= POSITION_ERROR_WHITE_KING_COUNT;
    }
    if (bitboard_popcount(expected_pieces[BK]) != 1) {
        status |= POSITION_ERROR_BLACK_KING_COUNT;
    }

    return status;
}