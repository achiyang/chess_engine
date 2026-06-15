#include "make_move.h"

static void clear_castling_rights(Position *pos, Square from, Square to) {
    switch (from) {
        case E1:
            pos->castling_rights &= ~(CASTLING_WHITE_KING | CASTLING_WHITE_QUEEN);
            break;
        case H1:
            pos->castling_rights &= ~(CASTLING_WHITE_KING);
            break;
        case A1:
            pos->castling_rights &= ~(CASTLING_WHITE_QUEEN);
            break;
        case E8:
            pos->castling_rights &= ~(CASTLING_BLACK_KING | CASTLING_BLACK_QUEEN);
            break;
        case H8:
            pos->castling_rights &= ~(CASTLING_BLACK_KING);
            break;
        case A8:
            pos->castling_rights &= ~(CASTLING_BLACK_QUEEN);
            break;
        default:
            break;
    }

    switch (to) {
        case H1:
            pos->castling_rights &= ~CASTLING_WHITE_KING;
            break;
        case A1:
            pos->castling_rights &= ~CASTLING_WHITE_QUEEN;
            break;
        case H8:
            pos->castling_rights &= ~CASTLING_BLACK_KING;
            break;
        case A8:
            pos->castling_rights &= ~CASTLING_BLACK_QUEEN;
            break;
        default:
            break;
    }
}

void position_make_move(Position *pos, Move move, PositionUndo *undo) {
    Color side = pos->side_to_move;
    Color enemy = opposite_side(side);

    Square from = move_from(move);
    Square to = move_to(move);
    MoveFlags flags = move_flags(move);

    Square capture_square =
        (flags & MOVE_FLAG_EN_PASSANT) ?
            (Square)((int)to + (side == WHITE ? -8 : 8)) :
            to;

    Piece moving_piece = pos->board[from];
    Piece captured_piece = pos->board[capture_square];

    undo->captured_piece  = captured_piece;
    undo->castling_rights = pos->castling_rights;
    undo->en_passant      = pos->en_passant;
    undo->halfmove_clock  = pos->halfmove_clock;
    undo->fullmove_number = pos->fullmove_number;
    
    if (flags & MOVE_FLAG_CAPTURE) {
        position_remove_piece(pos, capture_square);
    }

    if (flags & MOVE_FLAG_PROMOTION) {
        Piece promotion = piece_make(side, move_promotion(move));
        position_remove_piece(pos, from);
        position_set_piece(pos, promotion, to);
    }
    else {
        position_move_piece(pos, from, to);
    }

    if (flags & MOVE_FLAG_CASTLING) {
        switch (to) {
            case G1:
                position_move_piece(pos, H1, F1);
                break;
            case C1:
                position_move_piece(pos, A1, D1);
                break;
            case G8:
                position_move_piece(pos, H8, F8);
                break;
            case C8:
                position_move_piece(pos, A8, D8);
                break;
            default:
                break;
        }
    }

    clear_castling_rights(pos, from, to);

    pos->en_passant =
        (flags & MOVE_FLAG_DOUBLE) ?
            (Square)(((int)from + (int)to) / 2) :
            NO_SQUARE;
    pos->halfmove_clock =
        (piece_type_of(moving_piece) == PAWN || captured_piece != NO_PIECE) ?
            0 :
            undo->halfmove_clock + 1;
    pos->fullmove_number =
        (side == BLACK) ?
            undo->fullmove_number + 1 :
            undo->fullmove_number;
    pos->side_to_move = enemy;
}

void position_unmake_move(Position *pos, Move move, const PositionUndo *undo) {
    Color side = opposite_side(pos->side_to_move);

    pos->side_to_move    = side;
    pos->castling_rights = undo->castling_rights;
    pos->en_passant      = undo->en_passant;
    pos->halfmove_clock  = undo->halfmove_clock;
    pos->fullmove_number = undo->fullmove_number;

    Square from = move_from(move);
    Square to = move_to(move);
    MoveFlags flags = move_flags(move);

    Square capture_square = 
        (flags & MOVE_FLAG_EN_PASSANT) ?
            (Square)((int)to + (side == WHITE ? -8 : 8)) :
            to;

    Piece captured_piece = undo->captured_piece;

    if (flags & MOVE_FLAG_CASTLING) {
        switch (to) {
            case G1:
                position_move_piece(pos, F1, H1);
                break;
            case C1:
                position_move_piece(pos, D1, A1);
                break;
            case G8:
                position_move_piece(pos, F8, H8);
                break;
            case C8:
                position_move_piece(pos, D8, A8);
                break;
            default:
                break;
        }
    }

    if (flags & MOVE_FLAG_PROMOTION) {
        Piece pawn = piece_make(side, PAWN);

        position_remove_piece(pos, to);
        position_set_piece(pos, pawn, from);
    }
    else {
        position_move_piece(pos, to, from);
    }

    if (captured_piece != NO_PIECE) {
        position_set_piece(pos, captured_piece, capture_square);
    }
}