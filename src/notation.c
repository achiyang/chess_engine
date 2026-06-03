#include "notation.h"

Piece notation_piece_from_char(char c) {
    switch (c) {
        case 'P': return WP;
        case 'N': return WN;
        case 'B': return WB;
        case 'R': return WR;
        case 'Q': return WQ;
        case 'K': return WK;

        case 'p': return BP;
        case 'n': return BN;
        case 'b': return BB;
        case 'r': return BR;
        case 'q': return BQ;
        case 'k': return BK;

        default:  return NO_PIECE;
    }
}

char notation_piece_to_char(Piece piece) {
    switch (piece) {
        case WP: return 'P';
        case WN: return 'N';
        case WB: return 'B';
        case WR: return 'R';
        case WQ: return 'Q';
        case WK: return 'K';

        case BP: return 'p';
        case BN: return 'n';
        case BB: return 'b';
        case BR: return 'r';
        case BQ: return 'q';
        case BK: return 'k';

        default: return '.';
    }
}

Square notation_square_from_coord(const char *coord) {
    if (!coord) {
        return NO_SQUARE;
    }

    int file = coord[0] - 'a';
    int rank = coord[1] - '1';
    
    if (file < 0 || file >= FILE_NB || rank < 0 || rank >= RANK_NB) {
        return NO_SQUARE;
    }

    return make_square((File)file, (Rank)rank);
}

void notation_square_to_coord(Square square, char out[3]) {
    File file = square_file(square);
    Rank rank = square_rank(square);

    out[0] = 'a' + file;
    out[1] = '1' + rank;
    out[2] = '\0';
}

Color notation_side_from_char(char c) {
    return
        c == 'w' ? 
            WHITE :
        c == 'b' ?
            BLACK :
            NO_COLOR;
}

char notation_side_to_char(Color side) {
    return
        side == WHITE ?
            'w' :
        side == BLACK ?
            'b' :
            '\0';
}