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

static void add_pawn_promotions_from_targets(
    MoveList *list,
    Bitboard targets,
    int move_delta,
    MoveFlags flags
) {
    while (targets) {
        Square to = bitboard_pop_lsb(&targets);
        Square from = (Square)((int)to - move_delta);

        for (PieceType promotion = KNIGHT; promotion <= QUEEN; ++promotion) {
            move_list_add(
                list,
                move_make(
                    from,
                    to,
                    flags | MOVE_FLAG_PROMOTION,
                    promotion
                )
            );
        }
    }
}

static void add_pawn_moves_from_targets(
    MoveList *list,
    Bitboard targets,
    int move_delta,
    MoveFlags flags
) {
    while (targets) {
        Square to = bitboard_pop_lsb(&targets);
        Square from = (Square)((int)to - move_delta);

        move_list_add(
            list,
            move_make(
                from,
                to,
                flags,
                NO_PIECE_TYPE
            )
        );
    }
}

static void generate_white_pawn_moves(const Position *pos, MoveList *list) {
    Bitboard enemy_occ = pos->occupancies[BLACK];
    Bitboard all_occ = pos->occupancies[BOTH];
    Bitboard empty = ~all_occ;

    Bitboard pawns = pos->pieces[WP];

    /*
     * 1. single push
     */
    Bitboard single_targets = (pawns << 8) & empty;
    Bitboard normal_pushes = single_targets & ~BB_RANK_8;
    Bitboard promotion_pushes = single_targets & BB_RANK_8;

    /*
     * 2. double push
     */
    Bitboard one_step_from_start = ((pawns & BB_RANK_2) << 8) & empty;
    Bitboard double_targets = (one_step_from_start << 8) & empty;

    /*
     * 3. capture
     */
    Bitboard capture_west_targets = ((pawns & BB_NOT_FILE_A) << 7) & enemy_occ;
    Bitboard capture_east_targets = ((pawns & BB_NOT_FILE_H) << 9) & enemy_occ;

    Bitboard normal_capture_west = capture_west_targets & ~BB_RANK_8;
    Bitboard promo_capture_west  = capture_west_targets & BB_RANK_8;

    Bitboard normal_capture_east = capture_east_targets & ~BB_RANK_8;
    Bitboard promo_capture_east  = capture_east_targets & BB_RANK_8;
}

static void generate_black_pawn_moves(const Position *pos, MoveList *list) {
    Bitboard enemy_occ = pos->occupancies[WHITE];
    Bitboard all_occ = pos->occupancies[BOTH];
    Bitboard empty = ~all_occ;

    Bitboard pawns = pos->pieces[BP];

    /*
     * 1. single push
     */
    Bitboard single_targets = (pawns >> 8) & empty;
    Bitboard normal_pushes = single_targets & ~BB_RANK_1;
    Bitboard promotion_pushes = single_targets & BB_RANK_1;

    /*
     * 2. double push
     */
    Bitboard one_step_from_start = ((pawns & BB_RANK_7) >> 8) & empty;
    Bitboard double_targets = (one_step_from_start >> 8) & empty;

    /*
     * 3. capture
     */
    Bitboard capture_west_targets = ((pawns & BB_NOT_FILE_A) >> 9) & enemy_occ;
    Bitboard capture_east_targets = ((pawns & BB_NOT_FILE_H) >> 7) & enemy_occ;

    Bitboard normal_capture_west = capture_west_targets & ~BB_RANK_1;
    Bitboard promo_capture_west  = capture_west_targets & BB_RANK_1;

    Bitboard normal_capture_east = capture_east_targets & ~BB_RANK_1;
    Bitboard promo_capture_east  = capture_east_targets & BB_RANK_1;
}

static void generate_pawn_moves(const Position *pos, MoveList *list) {
    if (pos->side_to_move == WHITE) {
        generate_white_pawn_moves(pos, list);
    }
    else {
        generate_black_pawn_moves(pos, list);
    }
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