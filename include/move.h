#ifndef CHESS_ENGINE_MOVE_H
#define CHESS_ENGINE_MOVE_H

#include "types.h"

/*
 * from      : bits  0..5
 * to        : bits  6..11
 * flags     : bits 12..16
 * promotion : bits 17..19
*/
typedef uint32_t Move;

typedef uint32_t MoveFlags;

typedef enum MoveFlag {
    MOVE_FLAG_NONE       = 0,
    MOVE_FLAG_CAPTURE    = 1U << 0,
    MOVE_FLAG_DOUBLE     = 1U << 1,
    MOVE_FLAG_CASTLING   = 1U << 2,
    MOVE_FLAG_EN_PASSANT = 1U << 3,
    MOVE_FLAG_PROMOTION  = 1U << 4
} MoveFlag;
 
enum {
    MOVE_NONE = 0,

    MOVE_FROM_SHIFT      = 0,
    MOVE_TO_SHIFT        = 6,
    MOVE_FLAGS_SHIFT     = 12,
    MOVE_PROMOTION_SHIFT = 17,

    MOVE_SQUARE_MASK     = 0x3F,
    MOVE_FLAGS_MASK      = 0x1F,
    MOVE_PROMOTION_MASK  = 0x07
};

static inline Move move_make(
    Square from,
    Square to,
    MoveFlags flags,
    PieceType promotion
) {
    return
        (Move)from |
        (Move)to << MOVE_TO_SHIFT |
        (Move)flags << MOVE_FLAGS_SHIFT |
        (Move)promotion << MOVE_PROMOTION_SHIFT;
}

static inline Square move_from(Move move) {
    return (Square)((move >> MOVE_FROM_SHIFT) & MOVE_SQUARE_MASK);
}

static inline Square move_to(Move move) {
    return (Square)((move >> MOVE_TO_SHIFT) & MOVE_SQUARE_MASK);
}

static inline MoveFlags move_flags(Move move) {
    return (MoveFlags)((move >> MOVE_FLAGS_SHIFT) & MOVE_FLAGS_MASK);
}

static inline PieceType move_promotion(Move move) {
    return (PieceType)((move >> MOVE_PROMOTION_SHIFT) & MOVE_PROMOTION_MASK);
}

static inline int move_is_capture(Move move) {
    return (move_flags(move) & MOVE_FLAG_CAPTURE) != 0;
}

static inline int move_is_double(Move move) {
    return (move_flags(move) & MOVE_FLAG_DOUBLE) != 0;
}

static inline int move_is_castling(Move move) {
    return (move_flags(move) & MOVE_FLAG_CASTLING) != 0;
}

static inline int move_is_en_passant(Move move) {
    return (move_flags(move) & MOVE_FLAG_EN_PASSANT) != 0;
}

static inline int move_is_promotion(Move move) {
    return (move_flags(move) & MOVE_FLAG_PROMOTION) != 0;
}

#endif