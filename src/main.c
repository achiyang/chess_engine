#include <stdio.h>

#include "attacks.h"
#include "fen.h"
#include "movegen.h"
#include "position.h"

typedef struct MovegenTest {
    const char *name;
    const char *fen;

    int total;
    int captures;
    int doubles;
    int promotions;
    int en_passants;
} MovegenTest;

static int count_captures(const MoveList *list) {
    int count = 0;

    for (int i = 0; i < list->count; ++i) {
        if (move_is_capture(list->moves[i])) {
            ++count;
        }
    }

    return count;
}

static int count_doubles(const MoveList *list) {
    int count = 0;

    for (int i = 0; i < list->count; ++i) {
        if (move_is_double(list->moves[i])) {
            ++count;
        }
    }

    return count;
}

static int count_promotions(const MoveList *list) {
    int count = 0;

    for (int i = 0; i < list->count; ++i) {
        if (move_is_promotion(list->moves[i])) {
            ++count;
        }
    }

    return count;
}

static int count_en_passants(const MoveList *list) {
    int count = 0;

    for (int i = 0; i < list->count; ++i) {
        if (move_is_en_passant(list->moves[i])) {
            ++count;
        }
    }

    return count;
}

static int run_movegen_test(const MovegenTest *test) {
    Position pos;

    FenResult fen_result = fen_parse(&pos, test->fen);
    if (fen_result != FEN_OK) {
        printf("[FAIL] %s: fen error %d\n", test->name, fen_result);
        return 0;
    }

    PositionStatus status = position_validate(&pos);
    if (status != POSITION_STATUS_OK) {
        printf("[FAIL] %s: position invalid %u\n", test->name, (unsigned)status);
        return 0;
    }

    MoveList list;
    movegen_generate_pseudo_legal(&pos, &list);

    int captures = count_captures(&list);
    int doubles = count_doubles(&list);
    int promotions = count_promotions(&list);
    int en_passants = count_en_passants(&list);

    if (list.count != test->total ||
        captures != test->captures ||
        doubles != test->doubles ||
        promotions != test->promotions ||
        en_passants != test->en_passants) {
        printf("[FAIL] %s\n", test->name);
        printf("  total      : got %d, expected %d\n", list.count, test->total);
        printf("  captures   : got %d, expected %d\n", captures, test->captures);
        printf("  doubles    : got %d, expected %d\n", doubles, test->doubles);
        printf("  promotions : got %d, expected %d\n", promotions, test->promotions);
        printf("  en passant : got %d, expected %d\n", en_passants, test->en_passants);
        return 0;
    }

    printf("[OK] %s\n", test->name);
    return 1;
}

int main(void) {
    attacks_init();

    static const MovegenTest tests[] = {
        {
            "startpos white",
            STARTPOS_FEN,
            20, 0, 8, 0, 0
        },
        {
            "startpos black",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
            20, 0, 8, 0, 0
        },
        {
            "white promotion push",
            "6k1/P7/8/8/8/8/8/6K1 w - - 0 1",
            9, 0, 0, 4, 0
        },
        {
            "white promotion capture",
            "1r4k1/P7/8/8/8/8/8/6K1 w - - 0 1",
            13, 4, 0, 8, 0
        },
        {
            "white en passant",
            "6k1/8/8/3pP3/8/8/8/6K1 w - d6 0 1",
            7, 1, 0, 0, 1
        },
        {
            "black en passant",
            "6k1/8/8/8/3Pp3/8/8/6K1 b - d3 0 1",
            7, 1, 0, 0, 1
        },
        {
            "rook blockers",
            "6k1/8/3p4/8/3R4/8/3P4/6K1 w - - 0 1",
            16, 1, 0, 0, 0
        }
    };

    int pass = 1;
    int count = (int)(sizeof(tests) / sizeof(tests[0]));

    for (int i = 0; i < count; ++i) {
        if (!run_movegen_test(&tests[i])) {
            pass = 0;
        }
    }

    return pass ? 0 : 1;
}