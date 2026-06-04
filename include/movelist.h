#ifndef CHESS_ENGINE_MOVELIST_H
#define CHESS_ENGINE_MOVELIST_H

#include "move.h"

/*
 * 이 모듈의 helper 함수들은 별도의 runtime validation을 하지 않는다.
 * 각 함수의 Precondition은 caller가 보장해야 한다.
 */

enum {
    MOVE_LIST_CAPACITY = 256
};

typedef struct MoveList {
    Move moves[MOVE_LIST_CAPACITY];
    int count;
} MoveList;

/*
 * Precondition:
 * - list != NULL
 *
 * Effect:
 * - list를 empty 상태로 초기화한다.
 */
static inline void move_list_clear(MoveList *list) {
    list->count = 0;
}

/*
 * Precondition:
 * - list != NULL
 * - list->count < MOVE_LIST_CAPACITY
 *
 * Effect:
 * - list에 move를 추가한다.
 */
static inline void move_list_add(MoveList *list, Move move) {
    list->moves[list->count++] = move;
}

#endif