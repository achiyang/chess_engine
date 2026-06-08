#include <stddef.h>
#include <stdio.h>

#include "position.h"
#include "position_debug.h"
#include "fen.h"
#include "attacks.h"

typedef struct AttackTest {
    const char *name;
    const char *fen;
    int expected_white_check;
    int expected_black_check;
} AttackTest;

static const AttackTest attack_tests[] = {
    {
        "startpos",
        STARTPOS_FEN,
        0,
        0
    },
    {
        "bare kings no check",
        "6k1/8/8/8/8/8/8/4K3 w - - 0 1",
        0,
        0
    },

    /* rook / file / rank */
    {
        "black rook file check",
        "4r1k1/8/8/8/8/8/8/4K3 w - - 0 1",
        1,
        0
    },
    {
        "black rook rank check",
        "6k1/8/8/8/8/8/8/r3K3 w - - 0 1",
        1,
        0
    },
    {
        "black rook blocked by white piece",
        "4r1k1/8/8/8/4P3/8/8/4K3 w - - 0 1",
        0,
        0
    },
    {
        "black rook blocked by black piece",
        "4r1k1/8/8/8/4n3/8/8/4K3 w - - 0 1",
        0,
        0
    },
    {
        "white rook file check",
        "4k3/8/8/8/8/8/8/4R1K1 b - - 0 1",
        0,
        1
    },

    /* bishop / diagonal */
    {
        "black bishop diagonal check",
        "6k1/8/8/8/7b/8/8/4K3 w - - 0 1",
        1,
        0
    },
    {
        "black bishop other diagonal check",
        "6k1/8/8/8/1b6/8/8/4K3 w - - 0 1",
        1,
        0
    },
    {
        "black bishop blocked by white piece",
        "6k1/8/8/8/7b/6P1/8/4K3 w - - 0 1",
        0,
        0
    },
    {
        "black bishop blocked by black piece",
        "6k1/8/8/8/7b/6n1/8/4K3 w - - 0 1",
        0,
        0
    },
    {
        "white bishop diagonal check",
        "4k3/8/8/8/8/2B5/8/6K1 b - - 0 1",
        0,
        1
    },

    /* queen */
    {
        "black queen diagonal check",
        "6k1/8/8/8/8/8/2q5/4K3 w - - 0 1",
        1,
        0
    },
    {
        "black queen file check",
        "6k1/8/8/8/4q3/8/8/4K3 w - - 0 1",
        1,
        0
    },
    {
        "black queen rank check",
        "6k1/8/8/8/8/8/8/q3K3 w - - 0 1",
        1,
        0
    },
    {
        "black queen blocked",
        "6k1/8/8/8/4q3/4P3/8/4K3 w - - 0 1",
        0,
        0
    },

    /* knight */
    {
        "black knight check",
        "6k1/8/5n2/8/4K3/8/8/8 w - - 0 1",
        1,
        0
    },
    {
        "black knight edge check",
        "6k1/8/8/8/8/2n5/8/4K3 w - - 0 1",
        1,
        0
    },
    {
        "white knight check",
        "4k3/8/5N2/8/8/8/8/6K1 b - - 0 1",
        0,
        1
    },
    {
        "knight no check",
        "6k1/8/8/5n2/4K3/8/8/8 w - - 0 1",
        0,
        0
    },

    /* pawn */
    {
        "black pawn check from d3",
        "6k1/8/8/8/8/3p4/4K3/8 w - - 0 1",
        1,
        0
    },
    {
        "black pawn check from f3",
        "6k1/8/8/8/8/5p2/4K3/8 w - - 0 1",
        1,
        0
    },
    {
        "white pawn check from f7",
        "6k1/5P2/8/8/8/8/8/4K3 b - - 0 1",
        0,
        1
    },
    {
        "white pawn check from d7",
        "4k3/3P4/8/8/8/8/8/6K1 b - - 0 1",
        0,
        1
    },
    {
        "pawn no check wrong direction",
        "6k1/8/8/8/8/4p3/8/4K3 w - - 0 1",
        0,
        0
    },

    /* edge / corner sliding */
    {
        "corner rook check",
        "k7/8/8/8/8/8/8/R6K b - - 0 1",
        0,
        1
    },
    {
        "corner bishop check",
        "k7/8/8/8/8/8/1B6/7K b - - 0 1",
        0,
        1
    },
    {
        "long diagonal black bishop check",
        "6k1/8/8/8/8/8/1b6/4K3 w - - 0 1",
        0,
        0
    },
    {
        "long diagonal black bishop check from h4",
        "6k1/8/8/8/7b/8/8/4K3 w - - 0 1",
        1,
        0
    }
};

static int run_attack_test(const AttackTest *test) {
    Position pos;

    FenResult fen_result = fen_parse(&pos, test->fen);
    if (fen_result != FEN_OK) {
        printf("[FAIL] %s: fen error: %d\n", test->name, fen_result);
        return 0;
    }

    PositionStatus pos_status = position_validate(&pos);
    if (pos_status != POSITION_STATUS_OK) {
        printf("[FAIL] %s: position invalid: %u\n",
            test->name,
            (unsigned)pos_status
        );
        position_print(&pos, stdout);
        return 0;
    }

    int white_check = position_is_in_check(&pos, WHITE);
    int black_check = position_is_in_check(&pos, BLACK);

    if (white_check != test->expected_white_check ||
        black_check != test->expected_black_check) {
        printf("[FAIL] %s\n", test->name);
        printf("  expected: white=%d black=%d\n",
            test->expected_white_check,
            test->expected_black_check
        );
        printf("  actual  : white=%d black=%d\n",
            white_check,
            black_check
        );
        position_print(&pos, stdout);
        return 0;
    }

    printf("[PASS] %s\n", test->name);
    return 1;
}

int main(void) {
    attacks_init();

    int passed = 0;
    int total = (int)(sizeof(attack_tests) / sizeof(attack_tests[0]));

    for (int i = 0; i < total; ++i) {
        passed += run_attack_test(&attack_tests[i]);
    }

    printf("\nresult: %d/%d passed\n", passed, total);

    return passed == total ? 0 : 1;
}