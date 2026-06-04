#ifndef CHESS_ENGINE_ATTACK_H
#define CHESS_ENGINE_ATTACK_H

#include "position.h"

/*
 * 이 모듈의 함수들은 별도의 runtime validation을 하지 않는다.
 * 각 함수의 Precondition은 caller가 보장해야 한다.
 */

/*
 * Effect:
 * - attack detection에 필요한 내부 attack table을 초기화한다.
 *
 * Note:
 * - position_is_square_attacked()를 호출하기 전에 먼저 호출해야 한다.
 */
void attack_init(void);

/*
 * Precondition:
 * - pos != NULL
 * - square_is_valid(square)
 * - side_is_valid(attacker)
 * - attack_init()이 먼저 호출되어 있어야 한다.
 *
 * Effect:
 * - square가 attacker side에 의해 attacked 상태인지 확인한다.
 *
 * Return:
 * - 공격받고 있으면 true, 아니면 false.
 */
int position_is_square_attacked(const Position *pos, Square square, Color attacker);

/*
 * Precondition:
 * - pos != NULL
 * - side_is_valid(side)
 * - side의 king이 정확히 하나 존재한다.
 * - attack_init()이 먼저 호출되어 있어야 한다.
 *
 * Effect:
 * - side의 king이 opposite side에 의해 attacked 상태인지 확인한다.
 *
 * Return:
 * - check 상태이면 true, 아니면 false.
 */
int position_is_in_check(const Position *pos, Color side);

#endif