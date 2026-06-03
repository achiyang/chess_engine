#include "fen.h"

#include "notation.h"

#include <stddef.h>
#include <ctype.h>
#include <limits.h>

static const char *skip_spaces(const char *str) {
    while (*str == ' ') {
        ++str;
    }

    return str;
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
        char c = *str;

        Piece piece = notation_piece_from_char(c);
        if (piece != NO_PIECE) {
            if (file >= FILE_NB) {
                return NULL;
            }

            Square square = make_square((File)file, (Rank)rank);
            position_set_piece(pos, piece, square);

            ++file; ++str;
        }

        else if (c >= '1' && c <= '8') {
            file += c - '0';

            if (file > FILE_NB) {
                return NULL;
            }

            ++str;
            if (isdigit((unsigned char)*str)) {
                return NULL;
            }
        }

        else if (c == '/') {
            if (file != FILE_NB) {
                return NULL;
            }

            if (rank == RANK_1) {
                return NULL;
            }

            file = FILE_A;
            --rank; ++str;
        }

        else if (c == ' ') {
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

    char c = *str;

    switch (c) {
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
        char c = *str;

        switch (c) {
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

    Square square = notation_square_from_coord(str);
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