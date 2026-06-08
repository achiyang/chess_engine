#ifndef CHESS_ENGINE_ATTACKS_H
#define CHESS_ENGINE_ATTACKS_H

#include "position.h"

/*
 * 이 모듈의 함수들은 별도의 runtime validation을 하지 않는다.
 * 각 함수의 Precondition은 caller가 보장해야 한다.
 */

/*
 * Effect:
 * - attack bitboard 계산에 필요한 내부 table을 초기화한다.
 *
 * Note:
 * - 이 모듈의 다른 함수를 호출하기 전에 먼저 호출해야 한다.
 */
void attacks_init(void);

/*
 * Precondition:
 * - side_is_valid(side)
 * - square_is_valid(from)
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Return:
 * - from square의 side pawn이 공격하는 square들의 bitboard.
 */
Bitboard attacks_pawn(Color side, Square from);

/*
 * Precondition:
 * - square_is_valid(from)
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Return:
 * - from square의 knight가 공격하는 square들의 bitboard.
 */
Bitboard attacks_knight(Square from);

/*
 * Precondition:
 * - square_is_valid(from)
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Return:
 * - from square의 king이 공격하는 square들의 bitboard.
 */
Bitboard attacks_king(Square from);

/*
 * Precondition:
 * - square_is_valid(from)
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Return:
 * - occupied를 blocker로 고려했을 때 from square의 bishop이 공격하는 square들의 bitboard.
 */
Bitboard attacks_bishop(Square from, Bitboard occupied);

/*
 * Precondition:
 * - square_is_valid(from)
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Return:
 * - occupied를 blocker로 고려했을 때 from square의 rook이 공격하는 square들의 bitboard.
 */
Bitboard attacks_rook(Square from, Bitboard occupied);

/*
 * Precondition:
 * - square_is_valid(from)
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Return:
 * - occupied를 blocker로 고려했을 때 from square의 queen이 공격하는 square들의 bitboard.
 */
Bitboard attacks_queen(Square from, Bitboard occupied);

/*
 * Precondition:
 * - pos != NULL
 * - square_is_valid(square)
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Return:
 * - 현재 position에서 square를 공격하는 piece들이 위치한 square들의 bitboard.
 */
Bitboard position_attackers_to(const Position *pos, Square square);

/*
 * Precondition:
 * - pos != NULL
 * - square_is_valid(square)
 * - side_is_valid(attacker)
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Return:
 * - square가 attacker side에 의해 attacked 상태이면 true, 아니면 false.
 */
int position_is_square_attacked(const Position *pos, Square square, Color attacker);

/*
 * Precondition:
 * - pos != NULL
 * - side_is_valid(side)
 * - side의 king이 정확히 하나 존재한다.
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Return:
 * - side의 king이 opposite side에 의해 attacked 상태이면 true, 아니면 false.
 */
int position_is_in_check(const Position *pos, Color side);

#endif