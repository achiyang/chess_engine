#include "movegen.h"

#include "attacks.h"
#include "bitboard.h"

static void add_moves_from_targets(
    MoveList *list,
    Square from,
    Bitboard targets,
    Bitboard enemy_occ
) {
    Bitboard quiet_moves = targets & ~enemy_occ;
    Bitboard capture_moves = targets & enemy_occ;

    while (capture_moves) {
        Square to = bitboard_pop_lsb(&capture_moves);

        move_list_add(
            list,
            move_make(
                from,
                to,
                MOVE_FLAG_CAPTURE,
                NO_PIECE_TYPE
            )
        );
    }

    while (quiet_moves) {
        Square to = bitboard_pop_lsb(&quiet_moves);

        move_list_add(
            list,
            move_make(
                from,
                to,
                MOVE_FLAG_NONE,
                NO_PIECE_TYPE
            )
        );
    }
}

static void generate_pawn_moves(const Position *pos, MoveList *list) {
    (void)pos;
    (void)list;
}

static void generate_knight_moves(const Position *pos, MoveList *list) {
    Color side = pos->side_to_move;
    Color enemy = opposite_side(side);

    Bitboard own_occ = pos->occupancies[side];
    Bitboard enemy_occ = pos->occupancies[enemy];

    Piece knight = piece_make(side, KNIGHT);
    Bitboard knights = pos->pieces[knight];

    while (knights) {
        Square from = bitboard_pop_lsb(&knights);

        Bitboard attacks = attacks_knight(from);
        Bitboard targets = attacks & ~own_occ;

        add_moves_from_targets(list, from, targets, enemy_occ);
    }
}

static void generate_bishop_moves(const Position *pos, MoveList *list) {
    Color side = pos->side_to_move;
    Color enemy = opposite_side(side);

    Bitboard own_occ = pos->occupancies[side];
    Bitboard enemy_occ = pos->occupancies[enemy];
    Bitboard all_occ = pos->occupancies[BOTH];

    Piece bishop = piece_make(side, BISHOP);
    Bitboard bishops = pos->pieces[bishop];

    while (bishops) {
        Square from = bitboard_pop_lsb(&bishops);

        Bitboard attacks = attacks_bishop(from, all_occ);
        Bitboard targets = attacks & ~own_occ;

        add_moves_from_targets(list, from, targets, enemy_occ);
    }
}

static void generate_rook_moves(const Position *pos, MoveList *list) {
    Color side = pos->side_to_move;
    Color enemy = opposite_side(side);

    Bitboard own_occ = pos->occupancies[side];
    Bitboard enemy_occ = pos->occupancies[enemy];
    Bitboard all_occ = pos->occupancies[BOTH];

    Piece rook = piece_make(side, ROOK);
    Bitboard rooks = pos->pieces[rook];

    while (rooks) {
        Square from = bitboard_pop_lsb(&rooks);

        Bitboard attacks = attacks_rook(from, all_occ);
        Bitboard targets = attacks & ~own_occ;

        add_moves_from_targets(list, from, targets, enemy_occ);
    }
}

static void generate_queen_moves(const Position *pos, MoveList *list) {
    Color side = pos->side_to_move;
    Color enemy = opposite_side(side);

    Bitboard own_occ = pos->occupancies[side];
    Bitboard enemy_occ = pos->occupancies[enemy];
    Bitboard all_occ = pos->occupancies[BOTH];

    Piece queen = piece_make(side, QUEEN);
    Bitboard queens = pos->pieces[queen];

    while (queens) {
        Square from = bitboard_pop_lsb(&queens);

        Bitboard attacks = attacks_queen(from, all_occ);
        Bitboard targets = attacks & ~own_occ;

        add_moves_from_targets(list, from, targets, enemy_occ);
    }
}

static void generate_king_moves(const Position *pos, MoveList *list) {
    Color side = pos->side_to_move;
    Color enemy = opposite_side(side);

    Bitboard own_occ = pos->occupancies[side];
    Bitboard enemy_occ = pos->occupancies[enemy];

    Piece king = piece_make(side, KING);
    Bitboard king_bb = pos->pieces[king];

    Square from = bitboard_lsb(king_bb);

    Bitboard attacks = attacks_king(from);
    Bitboard targets = attacks & ~own_occ;

    add_moves_from_targets(list, from, targets, enemy_occ);
}

void movegen_generate_pseudo_legal(const Position *pos, MoveList *list) {
    move_list_clear(list);

    generate_pawn_moves(pos, list);
    generate_knight_moves(pos, list);
    generate_bishop_moves(pos, list);
    generate_rook_moves(pos, list);
    generate_queen_moves(pos, list);
    generate_king_moves(pos, list);
}