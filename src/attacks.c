#include "attacks.h"

#include "bitboard.h"

#include <string.h>

enum {
    DIR_NW = 0,
    DIR_N  = 1,
    DIR_NE = 2,
    DIR_W  = 3,
    DIR_E  = 4,
    DIR_SW = 5,
    DIR_S  = 6,
    DIR_SE = 7,
    DIRECTION_NB = 8
};

typedef struct SquareDelta {
    int df;
    int dr;
} SquareDelta;

static Bitboard pawn_attacks[SIDE_NB][SQUARE_NB];
static Bitboard knight_attacks[SQUARE_NB];
static Bitboard king_attacks[SQUARE_NB];

static Bitboard ray_masks[DIRECTION_NB][SQUARE_NB];

static const SquareDelta pawn_deltas[SIDE_NB][2] = {
    {
        { -1,  1 }, { 1,  1 }
    },
    {
        { -1, -1 }, { 1, -1 }
    }
};

static const SquareDelta knight_deltas[8] = {
    { -1,  2 }, { 1,  2 },
    { -2,  1 }, { 2,  1 },
    { -2, -1 }, { 2, -1 },
    { -1, -2 }, { 1, -2 }
};

static const SquareDelta king_deltas[8] = {
    { -1,  1 }, { 0,  1 }, { 1,  1 },
    { -1,  0 },            { 1,  0 },
    { -1, -1 }, { 0, -1 }, { 1, -1 }
};

static const SquareDelta direction_deltas[DIRECTION_NB] = {
    { -1,  1 }, { 0,  1 }, { 1,  1 },
    { -1,  0 },            { 1,  0 },
    { -1, -1 }, { 0, -1 }, { 1, -1 }
};

static Square square_apply_delta(Square square, SquareDelta delta) {
    int file = square_file(square) + delta.df;
    int rank = square_rank(square) + delta.dr;

    if (!file_is_valid(file) || !rank_is_valid(rank)) {
        return NO_SQUARE;
    }

    return square_make((File)file, (Rank)rank);
}

static void init_pawn_attacks(void) {
    for (Color side = WHITE; side < SIDE_NB; ++side) {
        for (Square square = A1; square < SQUARE_NB; ++square) {
            for (int i = 0; i < 2; ++i) {
                Square attack_square = square_apply_delta(square, pawn_deltas[side][i]);

                if (attack_square != NO_SQUARE) {
                    pawn_attacks[side][square] |= square_bb(attack_square);
                }
            }
        }
    }
}

static void init_knight_attacks(void) {
    for (Square square = A1; square < SQUARE_NB; ++square) {
        for (int i = 0; i < 8; ++i) {
            Square attack_square = square_apply_delta(square, knight_deltas[i]);

            if (attack_square != NO_SQUARE) {
                knight_attacks[square] |= square_bb(attack_square);
            }
        }
    }
}

static void init_king_attacks(void) {
    for (Square square = A1; square < SQUARE_NB; ++square) {
        for (int i = 0; i < 8; ++i) {
            Square attack_square = square_apply_delta(square, king_deltas[i]);

            if (attack_square != NO_SQUARE) {
                king_attacks[square] |= square_bb(attack_square);
            }
        }
    }
}

static void init_ray_masks(void) {
    for (int i = 0; i < DIRECTION_NB; ++i) {
        SquareDelta delta = direction_deltas[i];

        for (Square square = A1; square < SQUARE_NB; ++square) {
            Square current = square_apply_delta(square, delta);

            while (current != NO_SQUARE) {
                ray_masks[i][square] |= square_bb(current);
                current = square_apply_delta(current, delta);
            }
        }
    }
}

static const int direction_increasing[DIRECTION_NB] = {
    1, 1, 1,
    0,    1,
    0, 0, 0
};

static Bitboard ray_attack(int dir, Square from, Bitboard occupied) {
    Bitboard ray = ray_masks[dir][from];
    Bitboard blockers = ray & occupied;

    if (blockers == 0) {
        return ray;
    }

    Square blocker = 
        direction_increasing[dir] ?
            bitboard_lsb(blockers) :
            bitboard_msb(blockers);

    return ray & ~ray_masks[dir][blocker];
}

void attacks_init(void) {
    memset(pawn_attacks, 0, sizeof(pawn_attacks));
    memset(knight_attacks, 0, sizeof(knight_attacks));
    memset(king_attacks, 0, sizeof(king_attacks));
    memset(ray_masks, 0, sizeof(ray_masks));

    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();
    init_ray_masks();
}

Bitboard attacks_pawn(Color side, Square from) {
    return pawn_attacks[side][from];
}

Bitboard attacks_knight(Square from) {
    return knight_attacks[from];
}

Bitboard attacks_king(Square from) {
    return king_attacks[from];
}

Bitboard attacks_bishop(Square from, Bitboard occupied) {
    return
        ray_attack(DIR_NW, from, occupied) |
        ray_attack(DIR_NE, from, occupied) |
        ray_attack(DIR_SW, from, occupied) |
        ray_attack(DIR_SE, from, occupied);
}

Bitboard attacks_rook(Square from, Bitboard occupied) {
    return
        ray_attack(DIR_N, from, occupied) |
        ray_attack(DIR_W, from, occupied) |
        ray_attack(DIR_E, from, occupied) |
        ray_attack(DIR_S, from, occupied);
}

Bitboard attacks_queen(Square from, Bitboard occupied) {
    return attacks_bishop(from, occupied) | attacks_rook(from, occupied);
}

Bitboard position_attackers_to(const Position *pos, Square square) {
    Bitboard occupied = pos->occupancies[BOTH];

    Bitboard white_pawns = pos->pieces[WP];
    Bitboard black_pawns = pos->pieces[BP];

    Bitboard knights =
        pos->pieces[WN] | pos->pieces[BN];

    Bitboard kings =
        pos->pieces[WK] | pos->pieces[BK];

    Bitboard bishops_or_queens =
        pos->pieces[WB] | pos->pieces[BB] |
        pos->pieces[WQ] | pos->pieces[BQ];

    Bitboard rooks_or_queens =
        pos->pieces[WR] | pos->pieces[BR] |
        pos->pieces[WQ] | pos->pieces[BQ];

    return
        (attacks_pawn(BLACK, square) & white_pawns) |
        (attacks_pawn(WHITE, square) & black_pawns) |
        (attacks_knight(square) & knights) |
        (attacks_king(square) & kings) |
        (attacks_bishop(square, occupied) & bishops_or_queens) |
        (attacks_rook(square, occupied) & rooks_or_queens);
}

int position_is_square_attacked(const Position *pos, Square square, Color attacker) {
    Bitboard occupied = pos->occupancies[BOTH];

    Piece pawn   = piece_make(attacker, PAWN);
    Piece knight = piece_make(attacker, KNIGHT);
    Piece bishop = piece_make(attacker, BISHOP);
    Piece rook   = piece_make(attacker, ROOK);
    Piece queen  = piece_make(attacker, QUEEN);
    Piece king   = piece_make(attacker, KING);
    
    if ((attacks_pawn(opposite_side(attacker), square) & pos->pieces[pawn]) != 0) {
        return 1;
    }
    if ((attacks_knight(square) & pos->pieces[knight]) != 0) {
        return 1;
    }
    if ((attacks_king(square) & pos->pieces[king]) != 0) {
        return 1;
    }
    if ((attacks_bishop(square, occupied) & (pos->pieces[bishop] | pos->pieces[queen])) != 0) {
        return 1;
    }
    if ((attacks_rook(square, occupied) & (pos->pieces[rook] | pos->pieces[queen])) != 0) {
        return 1;
    }
    
    return 0;
}

int position_is_in_check(const Position *pos, Color side) {
    Piece king = piece_make(side, KING);
    Square square = bitboard_lsb(pos->pieces[king]);
    Color attacker = opposite_side(side);

    return position_is_square_attacked(pos, square, attacker);
}