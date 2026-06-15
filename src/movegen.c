#include "movegen.h"

#include "attacks.h"
#include "bitboard.h"
#include "make_move.h"

enum {
    PROMOTION_TYPE_NB = 4
};

/* Move ordering을 고려해 queen promotion을 먼저 생성한다. */
static const PieceType promotion_types[PROMOTION_TYPE_NB] = {
    QUEEN, ROOK, BISHOP, KNIGHT
};

/*
 * targets는 own occupancy가 제거된 target bitboard여야 한다.
 */
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

/*
 * move_delta는 to - from을 의미한다.
 * 이 함수는 MOVE_FLAG_PROMOTION을 직접 추가한다.
 */
static void add_pawn_promotions_from_targets(
    MoveList *list,
    Bitboard targets,
    int move_delta,
    MoveFlags flags
) {
    while (targets) {
        Square to = bitboard_pop_lsb(&targets);
        Square from = (Square)((int)to - move_delta);

        for (int i = 0; i < PROMOTION_TYPE_NB; ++i) {
            move_list_add(
                list,
                move_make(
                    from,
                    to,
                    flags | MOVE_FLAG_PROMOTION,
                    promotion_types[i]
                )
            );
        }
    }
}

/*
 * move_delta는 to - from을 의미한다.
 * Promotion move는 처리하지 않는다.
 */
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

/*
 * En passant target square는 비어 있으므로 normal capture 경로와 별도로 생성한다.
 * King safety는 legal move filtering 단계에서 검사한다.
 */
static void add_en_passant_moves(
    const Position *pos,
    MoveList *list,
    Color side,
    Bitboard pawns
) {
    Square to = pos->en_passant;

    if (to == NO_SQUARE) {
        return;
    }

    Color enemy = opposite_side(side);
    Piece captured_pawn = piece_make(enemy, PAWN);

    Square captured_square =
        side == WHITE ?
            (Square)((int)to - 8) :
            (Square)((int)to + 8);

    if (pos->board[captured_square] != captured_pawn) {
        return;
    }

    Bitboard candidate_pawns =
        attacks_pawn(enemy, to) & pawns;

    while (candidate_pawns) {
        Square from = bitboard_pop_lsb(&candidate_pawns);

        move_list_add(
            list,
            move_make(
                from,
                to,
                MOVE_FLAG_CAPTURE | MOVE_FLAG_EN_PASSANT,
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

    Bitboard single_targets = (pawns << 8) & empty;
    Bitboard normal_pushes = single_targets & ~BB_RANK_8;
    Bitboard promotion_pushes = single_targets & BB_RANK_8;

    Bitboard one_step_from_start = ((pawns & BB_RANK_2) << 8) & empty;
    Bitboard double_targets = (one_step_from_start << 8) & empty;

    Bitboard capture_west_targets = ((pawns & BB_NOT_FILE_A) << 7) & enemy_occ;
    Bitboard capture_east_targets = ((pawns & BB_NOT_FILE_H) << 9) & enemy_occ;

    Bitboard normal_capture_west = capture_west_targets & ~BB_RANK_8;
    Bitboard promo_capture_west  = capture_west_targets & BB_RANK_8;

    Bitboard normal_capture_east = capture_east_targets & ~BB_RANK_8;
    Bitboard promo_capture_east  = capture_east_targets & BB_RANK_8;

    add_pawn_promotions_from_targets(
        list,
        promo_capture_west,
        7,
        MOVE_FLAG_CAPTURE
    );
    add_pawn_promotions_from_targets(
        list,
        promo_capture_east,
        9,
        MOVE_FLAG_CAPTURE
    );

    add_pawn_moves_from_targets(
        list,
        normal_capture_west,
        7,
        MOVE_FLAG_CAPTURE
    );
    add_pawn_moves_from_targets(
        list,
        normal_capture_east,
        9,
        MOVE_FLAG_CAPTURE
    );

    add_pawn_promotions_from_targets(
        list,
        promotion_pushes,
        8,
        MOVE_FLAG_NONE
    );

    add_pawn_moves_from_targets(
        list,
        normal_pushes,
        8,
        MOVE_FLAG_NONE
    );

    add_pawn_moves_from_targets(
        list,
        double_targets,
        16,
        MOVE_FLAG_DOUBLE
    );

    add_en_passant_moves(pos, list, WHITE, pawns);
}

static void generate_black_pawn_moves(const Position *pos, MoveList *list) {
    Bitboard enemy_occ = pos->occupancies[WHITE];
    Bitboard all_occ = pos->occupancies[BOTH];
    Bitboard empty = ~all_occ;

    Bitboard pawns = pos->pieces[BP];

    Bitboard single_targets = (pawns >> 8) & empty;
    Bitboard normal_pushes = single_targets & ~BB_RANK_1;
    Bitboard promotion_pushes = single_targets & BB_RANK_1;

    Bitboard one_step_from_start = ((pawns & BB_RANK_7) >> 8) & empty;
    Bitboard double_targets = (one_step_from_start >> 8) & empty;

    Bitboard capture_west_targets = ((pawns & BB_NOT_FILE_A) >> 9) & enemy_occ;
    Bitboard capture_east_targets = ((pawns & BB_NOT_FILE_H) >> 7) & enemy_occ;

    Bitboard normal_capture_west = capture_west_targets & ~BB_RANK_1;
    Bitboard promo_capture_west  = capture_west_targets & BB_RANK_1;

    Bitboard normal_capture_east = capture_east_targets & ~BB_RANK_1;
    Bitboard promo_capture_east  = capture_east_targets & BB_RANK_1;

    add_pawn_promotions_from_targets(
        list,
        promo_capture_west,
        -9,
        MOVE_FLAG_CAPTURE
    );
    add_pawn_promotions_from_targets(
        list,
        promo_capture_east,
        -7,
        MOVE_FLAG_CAPTURE
    );

    add_pawn_moves_from_targets(
        list,
        normal_capture_west,
        -9,
        MOVE_FLAG_CAPTURE
    );
    add_pawn_moves_from_targets(
        list,
        normal_capture_east,
        -7,
        MOVE_FLAG_CAPTURE
    );

    add_pawn_promotions_from_targets(
        list,
        promotion_pushes,
        -8,
        MOVE_FLAG_NONE
    );

    add_pawn_moves_from_targets(
        list,
        normal_pushes,
        -8,
        MOVE_FLAG_NONE
    );

    add_pawn_moves_from_targets(
        list,
        double_targets,
        -16,
        MOVE_FLAG_DOUBLE
    );

    add_en_passant_moves(pos, list, BLACK, pawns);
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

/*
 * Pseudo-legal 단계에서는 attacked square로 이동하는 king move도 생성한다.
 */
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

static void generate_castling_moves(const Position *pos, MoveList *list) {
    Color side = pos->side_to_move;
    Color attacker = opposite_side(side);

    if (side == WHITE &&
        pos->board[E1] == WK &&
        !position_is_square_attacked(pos, E1, attacker)) {
        if (pos->castling_rights & CASTLING_WHITE_KING) {
            if (pos->board[H1] == WR &&
                pos->board[F1] == NO_PIECE &&
                pos->board[G1] == NO_PIECE &&
                !position_is_square_attacked(pos, F1, attacker) &&
                !position_is_square_attacked(pos, G1, attacker)) {
                move_list_add(
                    list,
                    move_make(
                        E1,
                        G1,
                        MOVE_FLAG_CASTLING,
                        NO_PIECE_TYPE
                    )
                );
            }
        }

        if (pos->castling_rights & CASTLING_WHITE_QUEEN) {
            if (pos->board[A1] == WR &&
                pos->board[D1] == NO_PIECE &&
                pos->board[C1] == NO_PIECE &&
                pos->board[B1] == NO_PIECE &&
                !position_is_square_attacked(pos, D1, attacker) &&
                !position_is_square_attacked(pos, C1, attacker)) {
                move_list_add(
                    list,
                    move_make(
                        E1,
                        C1,
                        MOVE_FLAG_CASTLING,
                        NO_PIECE_TYPE
                    )
                );
            }
        }
    }

    if (side == BLACK &&
        pos->board[E8] == BK &&
        !position_is_square_attacked(pos, E8, attacker)) {
        if (pos->castling_rights & CASTLING_BLACK_KING) {
            if (pos->board[H8] == BR &&
                pos->board[F8] == NO_PIECE &&
                pos->board[G8] == NO_PIECE &&
                !position_is_square_attacked(pos, F8, attacker) &&
                !position_is_square_attacked(pos, G8, attacker)) {
                move_list_add(
                    list,
                    move_make(
                        E8,
                        G8,
                        MOVE_FLAG_CASTLING,
                        NO_PIECE_TYPE
                    )
                );
            }
        }

        if (pos->castling_rights & CASTLING_BLACK_QUEEN) {
            if (pos->board[A8] == BR &&
                pos->board[D8] == NO_PIECE &&
                pos->board[C8] == NO_PIECE &&
                pos->board[B8] == NO_PIECE &&
                !position_is_square_attacked(pos, D8, attacker) &&
                !position_is_square_attacked(pos, C8, attacker)) {
                move_list_add(
                    list,
                    move_make(
                        E8,
                        C8,
                        MOVE_FLAG_CASTLING,
                        NO_PIECE_TYPE
                    )
                );
            }
        }
    }
}

void movegen_generate_pseudo_legal(const Position *pos, MoveList *list) {
    move_list_clear(list);

    generate_pawn_moves(pos, list);
    generate_knight_moves(pos, list);
    generate_bishop_moves(pos, list);
    generate_rook_moves(pos, list);
    generate_queen_moves(pos, list);
    generate_king_moves(pos, list);
    generate_castling_moves(pos, list);
}

void movegen_generate_legal(Position *pos, MoveList *list) {
    MoveList pseudo_list;
    movegen_generate_pseudo_legal(pos, &pseudo_list);

    move_list_clear(list);

    Color side = pos->side_to_move;

    for (int i = 0; i < pseudo_list.count; ++i) {
        Move move = pseudo_list.moves[i];
        PositionUndo undo;

        position_make_move(pos, move, &undo);

        if (!position_is_in_check(pos, side)) {
            move_list_add(list, move);
        }

        position_unmake_move(pos, move, &undo);
    }
}