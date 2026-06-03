#include "attack.h"

typedef struct SquareDelta {
    int df;
    int dr;
} SquareDelta;

static Bitboard pawn_attackers[SIDE_NB][SQUARE_NB];
static Bitboard knight_attackers[SQUARE_NB];
static Bitboard king_attackers[SQUARE_NB];

static const SquareDelta pawn_attack_deltas[SIDE_NB][2] = {
    {
        { -1,  1 }, { 1,  1 }
    },
    {
        { -1, -1 }, { 1, -1 }
    }
};

static const SquareDelta knight_attack_deltas[8] = {
    { -1,  2 }, { 1,  2 },
    { -2,  1 }, { 2,  1 },
    { -2, -1 }, { 2, -1 },
    { -1, -2 }, { 1, -2 }
};

static const SquareDelta king_attack_deltas[8] = {
    { -1,  1 }, { 0,  1 }, { 1,  1 },
    { -1,  0 },            { 1,  0 },
    { -1, -1 }, { 0, -1 }, { 1, -1 }
};

static Square square_apply_delta(Square square, SquareDelta delta) {
    int file = square_file(square) + delta.df;
    int rank = square_rank(square) + delta.dr;

    if (file < FILE_A || file >= FILE_NB || rank < RANK_1 || rank >= RANK_NB) {
        return NO_SQUARE;
    }

    return make_square((File)file, (Rank)rank);
}

static void init_pawn_attackers(void) {
    for (Color side = WHITE; side < SIDE_NB; ++side) {
        for (Square square = A1; square < SQUARE_NB; ++square) {
            for (int i = 0; i < 2; ++i) {
                Square attack_square = square_apply_delta(square, pawn_attack_deltas[side][i]);

                if (attack_square != NO_SQUARE) {
                    pawn_attackers[side][attack_square] |= square_bb(square);
                }
            }
        }
    }
}

static void init_knight_attackers(void) {
    for (Square square = A1; square < SQUARE_NB; ++square) {
        for (int i = 0; i < 8; ++i) {
            Square attack_square = square_apply_delta(square, knight_attack_deltas[i]);

            if (attack_square != NO_SQUARE) {
                knight_attackers[attack_square] |= square_bb(square);
            }
        }
    }
}

static void init_king_attackers(void) {
    for (Square square = A1; square < SQUARE_NB; ++square) {
        for (int i = 0; i < 8; ++i) {
            Square attack_square = square_apply_delta(square, king_attack_deltas[i]);

            if (attack_square != NO_SQUARE) {
                king_attackers[attack_square] |= square_bb(square);
            }
        }
    }
}

void attack_init(void) {
    init_pawn_attackers();
    init_knight_attackers();
    init_king_attackers();
}

int position_is_square_attacked(const Position *pos, Square square, Color attacker) {
    Piece pawn = make_piece(attacker, PAWN);
    Piece knight = make_piece(attacker, KNIGHT);
    Piece king = make_piece(attacker, KING);

    if (pawn_attackers[attacker][square] & pos->pieces[pawn]) {
        return 1;
    }

    if (knight_attackers[square] & pos->pieces[knight]) {
        return 1;
    }

    if (king_attackers[square] & pos->pieces[king]) {
        return 1;
    }

    return 0;
}