#include "position_debug.h"

#include "notation.h"

void position_print(const Position *pos, FILE *out) {
    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        fprintf(out, "%d  ", rank + 1);

        for (int file = FILE_A; file < FILE_NB; ++file) {
            Square square = make_square((File)file, (Rank)rank);
            Piece piece = pos->board[square];

            fprintf(out, "%c ", notation_piece_to_char(piece));
        }

        fprintf(out, "\n");
    }
    fprintf(out, "\n\n   a b c d e f g h\n\n");

    fprintf(out, "side: %c\n", notation_side_to_char(pos->side_to_move));

    fprintf(out, "castling: ");
    if (pos->castling_rights == CASTLING_NONE) {
        fprintf(out, "-");
    }
    else {
        if (pos->castling_rights & CASTLING_WHITE_KING) {
            fprintf(out, "K");
        }
        if (pos->castling_rights & CASTLING_WHITE_QUEEN) {
            fprintf(out, "Q");
        }
        if (pos->castling_rights & CASTLING_BLACK_KING) {
            fprintf(out, "k");
        }
        if (pos->castling_rights & CASTLING_BLACK_QUEEN) {
            fprintf(out, "q");
        }
    }
    fprintf(out, "\n");

    fprintf(out, "en_passant: ");
    if (pos->en_passant == NO_SQUARE) {
        fprintf(out, "-\n");
    }
    else if (square_is_valid(pos->en_passant)) {
        char coord[3];
        notation_square_to_coord(pos->en_passant, coord);
        fprintf(out, "%s\n", coord);
    }
    else {
        fprintf(out, "?\n");
    }

    fprintf(out, "halfmove: %d\n", pos->halfmove_clock);

    fprintf(out, "fullmove: %d\n", pos->fullmove_number);
}