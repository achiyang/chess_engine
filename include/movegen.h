#ifndef CHESS_ENGINE_MOVEGEN_H
#define CHESS_ENGINE_MOVEGEN_H

#include "position.h"
#include "movelist.h"

/*
 * 이 모듈은 pseudo-legal move generation을 담당한다.
 * 함수들은 별도의 runtime validation을 하지 않으며,
 * caller는 valid Position과 Position invariant를 보장해야 한다.
 */

/*
 * Precondition:
 * - pos != NULL
 * - list != NULL
 * - Position invariant가 유지되어 있어야 한다.
 * - side_to_move가 valid side여야 한다.
 * - 양쪽 king이 각각 정확히 하나 존재해야 한다.
 * - en_passant state가 valid해야 한다.
 * - attacks_init()이 먼저 호출되어 있어야 한다.
 *
 * Effect:
 * - list를 clear한다.
 * - 현재 side_to_move의 pseudo-legal moves를 생성한다.
 *
 * Note:
 * - king safety는 검사하지 않는다.
 * - pinned piece move, illegal king move, illegal en passant move가 포함될 수 있다.
 * - 현재 구현은 castling move를 생성하지 않는다.
 */
void movegen_generate_pseudo_legal(const Position *pos, MoveList *list);

#endif