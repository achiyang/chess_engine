#include "fen.h"

#include <stddef.h>
#include <ctype.h>
#include <limits.h>

static const char *skip_spaces(const char *str) {
    while (*str == ' ') {
        ++str;
    }

    return str;
}

static Piece piece_from_char(char c) {
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

static Square square_from_coord(const char *coord) {
    if (!coord) {
        return NO_SQUARE;
    }

    File file;
    Rank rank;

    switch (coord[0]) {
        case 'a': file = FILE_A; break;
        case 'b': file = FILE_B; break;
        case 'c': file = FILE_C; break;
        case 'd': file = FILE_D; break;
        case 'e': file = FILE_E; break;
        case 'f': file = FILE_F; break;
        case 'g': file = FILE_G; break;
        case 'h': file = FILE_H; break;

        default: return NO_SQUARE;
    }

    switch (coord[1]) {
        case '1': rank = RANK_1; break;
        case '2': rank = RANK_2; break;
        case '3': rank = RANK_3; break;
        case '4': rank = RANK_4; break;
        case '5': rank = RANK_5; break;
        case '6': rank = RANK_6; break;
        case '7': rank = RANK_7; break;
        case '8': rank = RANK_8; break;

        default: return NO_SQUARE;
    }

    return make_square(file, rank);
}

static const char *parse_int(const char *str, int *out) {
    if (!str || !out || !isdigit((unsigned char)*str)) {
        return NULL;
    }

    int value = 0;

    while (isdigit((unsigned char)*str)) {
        int digit = *str - '0';

        if (value > (INT_MAX - digit) / 10) {
            return NULL;
        }

        value = value * 10 + digit;
        ++str;
    }

    *out = value;
    return str;
}

static const char *parse_piece_placement(Position *pos, const char *str) {
    if (!pos || !str) {
        return NULL;
    }

    int file = FILE_A;
    int rank = RANK_8;

    while (1) {
        char ch = *str;

        Piece piece = piece_from_char(ch);
        if (piece != NO_PIECE) {
            if (file >= FILE_NB) {
                return NULL;
            }

            Square square = make_square((File)file, (Rank)rank);
            position_set_piece(pos, piece, square);

            ++file; ++str;
        }

        else if (ch >= '1' && ch <= '8') {
            file += ch - '0';

            if (file > FILE_NB) {
                return NULL;
            }

            ++str;
            if (isdigit((unsigned char)*str)) {
                return NULL;
            }
        }

        else if (ch == '/') {
            if (file != FILE_NB) {
                return NULL;
            }

            if (rank == RANK_1) {
                return NULL;
            }

            file = FILE_A;
            --rank; ++str;
        }

        else if (ch == ' ') {
            if (file == FILE_NB && rank == RANK_1) {
                return str;
            }

            return NULL;
        }

        else return NULL;
    }
}

static const char *parse_side_to_move(Position *pos, const char *str) {
    if (!pos || !str) {
        return NULL;
    }

    char ch = *str;

    switch (ch) {
        case 'w':
            pos->side_to_move = WHITE;
            break;
        case 'b':
            pos->side_to_move = BLACK;
            break;
        default:
            return NULL;
    }

    return (str + 1);
}

static const char *parse_castling_rights(Position *pos, const char *str) {
    if (!pos || !str) {
        return NULL;
    }

    pos->castling_rights = CASTLING_NONE;

    while (1) {
        char ch = *str;

        switch (ch) {
            case '-':
                if (pos->castling_rights != CASTLING_NONE) {
                    return NULL;
                }
                return (str + 1);

            case 'K':
                if (pos->castling_rights & CASTLING_WHITE_KING) {
                    return NULL;
                }
                pos->castling_rights |= CASTLING_WHITE_KING;
                break;

            case 'Q':
                if (pos->castling_rights & CASTLING_WHITE_QUEEN) {
                    return NULL;
                }
                pos->castling_rights |= CASTLING_WHITE_QUEEN;
                break;

            case 'k':
                if (pos->castling_rights & CASTLING_BLACK_KING) {
                    return NULL;
                }
                pos->castling_rights |= CASTLING_BLACK_KING;
                break;

            case 'q':
                if (pos->castling_rights & CASTLING_BLACK_QUEEN) {
                    return NULL;
                }
                pos->castling_rights |= CASTLING_BLACK_QUEEN;
                break;

            case ' ':
                if (pos->castling_rights == CASTLING_NONE) {
                    return NULL;
                }
                return str;

            default:
                return NULL;
        }

        ++str;
    }
}

static const char *parse_en_passant(Position *pos, const char *str) {
    if (!pos || !str) {
        return NULL;
    }

    pos->en_passant = NO_SQUARE;

    if (*str == '-') {
        return (str + 1);
    }

    Square square = square_from_coord(str);
    if (square == NO_SQUARE) {
        return NULL;
    }
    if (pos->board[square] != NO_PIECE) {
        return NULL;
    }

    Rank rank = square_rank(square);
    if (rank != (pos->side_to_move == WHITE ? RANK_6 : RANK_3)) {
        return NULL;
    }

    pos->en_passant = square;

    return (str + 2);
}

static const char *parse_halfmove_clock(Position *pos, const char *str) {
    if (!pos || !str) {
        return NULL;
    }

    return parse_int(str, &pos->halfmove_clock);
}

static const char *parse_fullmove_number(Position *pos, const char *str) {
    if (!pos || !str) {
        return NULL;
    }

    str = parse_int(str, &pos->fullmove_number);

    return (str && pos->fullmove_number > 0) ?
        str :
        NULL;
}

FenResult fen_parse(Position *pos, const char *fen) {
    if (!pos || !fen) {
        return FEN_ERROR_NULL;
    }

    Position temp;
    position_clear(&temp);

    fen = skip_spaces(fen);
    fen = parse_piece_placement(&temp, fen);
    if (!fen || *fen != ' ') {
        return FEN_ERROR_BOARD;
    }

    fen = skip_spaces(fen);
    fen = parse_side_to_move(&temp, fen);
    if (!fen || *fen != ' ') {
        return FEN_ERROR_SIDE;
    }

    fen = skip_spaces(fen);
    fen = parse_castling_rights(&temp, fen);
    if (!fen || *fen != ' ') {
        return FEN_ERROR_CASTLING;
    }

    fen = skip_spaces(fen);
    fen = parse_en_passant(&temp, fen);
    if (!fen || *fen != ' ') {
        return FEN_ERROR_EN_PASSANT;
    }

    fen = skip_spaces(fen);
    fen = parse_halfmove_clock(&temp, fen);
    if (!fen || *fen != ' ') {
        return FEN_ERROR_HALFMOVE;
    }

    fen = skip_spaces(fen);
    fen = parse_fullmove_number(&temp, fen);
    if (!fen) {
        return FEN_ERROR_FULLMOVE;
    }

    fen = skip_spaces(fen);
    if (*fen != '\0') {
        return FEN_ERROR_TRAILING;
    }

    *pos = temp;
    return FEN_OK;
}