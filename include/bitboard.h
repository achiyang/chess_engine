#ifndef CHESS_ENGINE_BITBOARD_H
#define CHESS_ENGINE_BITBOARD_H

#include "types.h"

/*
 * 이 모듈의 helper 함수들은 별도의 runtime validation을 하지 않는다.
 * 각 함수의 Precondition은 caller가 보장해야 한다.
 */

/*
 * Effect:
 * - bb가 비어 있는지 확인한다.
 *
 * Return:
 * - bb == 0이면 true, 아니면 false.
 */
static inline int bitboard_is_empty(Bitboard bb) {
    return bb == 0;
}

/*
 * Precondition:
 * - square_is_valid(square)
 *
 * Effect:
 * - bb에 square bit가 포함되어 있는지 확인한다.
 *
 * Return:
 * - square bit가 set되어 있으면 true, 아니면 false.
 */
static inline int bitboard_has_square(Bitboard bb, Square square) {
    return (bb & square_bb(square)) != 0;
}

/*
 * Precondition:
 * - bb != 0
 *
 * Effect:
 * - bb의 least significant set bit에 해당하는 square를 구한다.
 *
 * Return:
 * - least significant set bit의 Square.
 */
static inline Square bitboard_lsb(Bitboard bb) {
#if defined(__clang__) || defined(__GNUC__)
    return (Square)(__builtin_ctzll((unsigned long long)bb));
#else
    static const Square index64[64] = {
        0, 1, 48, 2, 57, 49, 28, 3,
        61, 58, 50, 42, 38, 29, 17, 4,
        62, 55, 59, 36, 53, 51, 43, 22,
        45, 39, 33, 30, 24, 18, 12, 5,
        63, 47, 56, 27, 60, 41, 37, 16,
        54, 35, 52, 21, 44, 32, 23, 11,
        46, 26, 40, 15, 34, 20, 31, 10,
        25, 14, 19, 9, 13, 8, 7, 6
    };

    Bitboard isolated = bb & -bb;
    return index64[(isolated * 0x03f79d71b4cb0a89ULL) >> 58];
#endif
}

/*
 * Precondition:
 * - bb != NULL
 * - *bb != 0
 *
 * Effect:
 * - *bb의 least significant set bit를 제거한다.
 *
 * Return:
 * - 제거된 bit에 해당하는 Square.
 */
static inline Square bitboard_pop_lsb(Bitboard *bb) {
    Square square = bitboard_lsb(*bb);

    *bb &= (*bb - 1);

    return square;
}

/*
 * Effect:
 * - bb에 set된 bit 개수를 센다.
 *
 * Return:
 * - set bit count.
 */
static inline int bitboard_popcount(Bitboard bb) {
#if defined(__clang__) || defined(__GNUC__)
    return __builtin_popcountll((unsigned long long)bb);
#else
    int count = 0;

    while (bb) {
        bb &= bb - 1;
        ++count;
    }

    return count;
#endif
}

#endif