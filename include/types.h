#ifndef CHESS_ENGINE_TYPES_H
#define CHESS_ENGINE_TYPES_H

#include <stdint.h>

typedef uint64_t Bitboard;

typedef enum Color {
    WHITE    = 0,
    BLACK    = 1,
    SIDE_NB  = 2,
    BOTH     = SIDE_NB,
    COLOR_NB = 3,
    NO_COLOR = COLOR_NB
} Color;

typedef enum Square {
    A1 =  0, B1 =  1, C1 =  2, D1 =  3, E1 =  4, F1 =  5, G1 =  6, H1 =  7,
    A2 =  8, B2 =  9, C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15,
    A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23,
    A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31,
    A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39,
    A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47,
    A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55,
    A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63,

    SQUARE_NB = 64,
    NO_SQUARE = SQUARE_NB
} Square;

typedef enum File {
    FILE_A  = 0,
    FILE_B  = 1,
    FILE_C  = 2,
    FILE_D  = 3,
    FILE_E  = 4,
    FILE_F  = 5,
    FILE_G  = 6,
    FILE_H  = 7,
    FILE_NB = 8,
    NO_FILE = FILE_NB
} File;

typedef enum Rank {
    RANK_1  = 0,
    RANK_2  = 1,
    RANK_3  = 2,
    RANK_4  = 3,
    RANK_5  = 4,
    RANK_6  = 5,
    RANK_7  = 6,
    RANK_8  = 7,
    RANK_NB = 8,
    NO_RANK = RANK_NB
} Rank;

typedef enum Piece {
    WP = 0,
    WN = 1,
    WB = 2,
    WR = 3,
    WQ = 4,
    WK = 5,

    BP = 6,
    BN = 7,
    BB = 8,
    BR = 9,
    BQ = 10,
    BK = 11,

    PIECE_NB = 12,
    NO_PIECE = PIECE_NB
} Piece;

typedef enum PieceType {
    PAWN   = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK   = 3,
    QUEEN  = 4,
    KING   = 5,

    PIECE_TYPE_NB = 6,
    NO_PIECE_TYPE = PIECE_TYPE_NB
} PieceType;

typedef uint8_t CastlingRights;

enum CastlingRight {
    CASTLING_NONE        = 0,
    CASTLING_WHITE_KING  = 1U << 0,
    CASTLING_WHITE_QUEEN = 1U << 1,
    CASTLING_BLACK_KING  = 1U << 2,
    CASTLING_BLACK_QUEEN = 1U << 3,
    CASTLING_ALL         =
        CASTLING_WHITE_KING |
        CASTLING_WHITE_QUEEN |
        CASTLING_BLACK_KING |
        CASTLING_BLACK_QUEEN
};

static inline int color_is_valid(int color) {
    return color >= WHITE && color < COLOR_NB;
}

static inline int side_is_valid(int side) {
    return side == WHITE || side == BLACK;
}

static inline Color opposite_side(Color side) {
    return (Color)(side ^ 1);
}

static inline int square_is_valid(int square) {
    return square >= A1 && square < SQUARE_NB;
}

static inline int file_is_valid(int file) {
    return file >= FILE_A && file < FILE_NB;
}

static inline int rank_is_valid(int rank) {
    return rank >= RANK_1 && rank < RANK_NB;
}

static inline File square_file(Square square) {
    return (File)(square & 7);
}

static inline Rank square_rank(Square square) {
    return (Rank)(square >> 3);
}

static inline Square square_make(File file, Rank rank) {
    return (Square)((rank << 3) | file);
}

static inline Bitboard square_bb(Square square) {
    return (Bitboard)(1ULL << square);
}

static inline int piece_is_valid(int piece) {
    return piece >= WP && piece < PIECE_NB;
}

static inline PieceType piece_type_of(Piece piece) {
    return (PieceType)(piece % PIECE_TYPE_NB);
}

static inline Color piece_color_of(Piece piece) {
    return (Color)(piece / PIECE_TYPE_NB);
}

static inline Piece piece_make(Color side, PieceType piece_type) {
    return (Piece)(piece_type + (side * PIECE_TYPE_NB));
}

static inline int castling_rights_is_valid(CastlingRights rights) {
    return (rights & ~CASTLING_ALL) == 0;
}

#endif