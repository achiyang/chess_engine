#include <stdio.h>

#include "position.h"
#include "position_debug.h"
#include "fen.h"
#include "attack.h"

int main(void) {
    attack_init();

    Position pos;

    FenResult fen_result = fen_parse(&pos, STARTPOS_FEN);
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

    return 0;
}