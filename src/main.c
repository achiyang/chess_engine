#include <stdio.h>

#include "position.h"
#include "position_debug.h"
#include "fen.h"
#include "attack.h"

static const char *fen_list[] = {
    STARTPOS_FEN,
    "6k1/8/8/8/8/8/8/4rK2 w - - 0 1",
    "6k1/8/8/8/7b/8/8/4K3 w - - 0 1",
    "6k1/8/5n2/8/4K3/8/8/8 w - - 0 1",
    "6k1/8/8/8/8/3p4/4K3/8 w - - 0 1",
    "6k1/4r3/8/8/4P3/8/8/4K3 w - - 0 1"
};

int main(void) {
    attack_init();

    Position pos;

    for (size_t i = 0; i < sizeof(fen_list) / sizeof(*fen_list); ++i) {
        FenResult fen_result = fen_parse(&pos, fen_list[i]);
        if (fen_result != FEN_OK) {
            printf("fen error: %d\n", fen_result);
            return 1;
        }

        PositionStatus pos_stat = position_validate(&pos);
        if (pos_stat != POSITION_STATUS_OK) {
            printf("position invalid: %u\n", (unsigned)pos_stat);
            return 1;
        }

        position_print(&pos, stdout);

        printf("white in check: %d\n", position_is_in_check(&pos, WHITE));
        printf("black in check: %d\n", position_is_in_check(&pos, BLACK));
        printf("\n\n");
    }

    return 0;
}