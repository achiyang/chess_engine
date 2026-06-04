#include "attacks.h"

#include "bitboard.h"

static Bitboard pawn_attacks[SIDE_NB][SQUARE_NB];
static Bitboard knight_attacks[SQUARE_NB];
static Bitboard king_attacks[SQUARE_NB];

typedef struct SquareDelta {
    int df;
    int dr;
} SquareDelta;

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

static const SquareDelta bishop_directions[4] = {
    { -1,  1 }, { 1,  1 },
    { -1, -1 }, { 1, -1 }
};

static const SquareDelta rook_directions[4] = {
    { 0,  1 },
    { -1, 0 }, { 1, 0 },
    { 0, -1 }
};

static Square square_apply_delta(Square square, SquareDelta delta) {
    int file = square_file(square) + delta.df;
    int rank = square_rank(square) + delta.dr;

    if (file < FILE_A || file >= FILE_NB || rank < RANK_1 || rank >= RANK_NB) {
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

static Bitboard attacks_sliding(
    Square from,
    Bitboard occupied,
    SquareDelta directions,
    int direction_count
) {
    
}

void attacks_init(void) {
    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();
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

}

Bitboard attacks_rook(Square from, Bitboard occupied) {

}

Bitboard attacks_queen(Square from, Bitboard occupied) {

}

Bitboard position_attackers_to(const Position *pos, Square square) {

}

int position_is_square_attacked(const Position *pos, Square square, Color attacker) {

}

int position_is_in_check(const Position *pos, Color side) {

}

/*
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

static int is_attacked_by_slider(
    const Position *pos,
    Square square,
    const SquareDelta directions[],
    int direction_count,
    Piece slider,
    Piece queen
) {
    for (int dir = 0; dir < direction_count; ++dir) {
        Square current = square_apply_delta(square, directions[dir]);

        while (current != NO_SQUARE) {
            Piece piece = pos->board[current];

            if (piece != NO_PIECE) {
                if (piece == slider || piece == queen) {
                    return 1;
                }
                else {
                    break;
                }
            }

            current = square_apply_delta(current, directions[dir]);
        }
    }

    return 0;
}

void attack_init(void) {
    init_pawn_attackers();
    init_knight_attackers();
    init_king_attackers();
}

int position_is_square_attacked(const Position *pos, Square square, Color attacker) {
    Piece pawn = piece_make(attacker, PAWN);
    Piece knight = piece_make(attacker, KNIGHT);
    Piece bishop = piece_make(attacker, BISHOP);
    Piece rook = piece_make(attacker, ROOK);
    Piece queen = piece_make(attacker, QUEEN);
    Piece king = piece_make(attacker, KING);

    if (pawn_attackers[attacker][square] & pos->pieces[pawn]) {
        return 1;
    }

    if (knight_attackers[square] & pos->pieces[knight]) {
        return 1;
    }

    if (king_attackers[square] & pos->pieces[king]) {
        return 1;
    }

    if (is_attacked_by_slider(pos, square, bishop_directions, 4, bishop, queen)) {
        return 1;
    }

    if (is_attacked_by_slider(pos, square, rook_directions, 4, rook, queen)) {
        return 1;
    }

    return 0;
}

int position_is_in_check(const Position *pos, Color side) {
    Piece king = piece_make(side, KING);
    Bitboard king_bb = pos->pieces[king];
    Square square = bitboard_lsb(king_bb);
    Color attacker = opposite_side(side);

    return position_is_square_attacked(pos, square, attacker);
}
*/