#ifndef __CMD_FRAMEWORK_HPP__
#define __CMD_FRAMEWORK_HPP__

#include "utils.hpp"

// gui(?) of cmd mode lol
namespace mfwu {

template <BoardSize Size=BoardSize::Small>
class DisplayFramework {
    //  board: 2 * 3
    /*
            A B C 
          . . . . .
        A . O + X .
        B . + O + .
          . . . . .

        framework:   row : 1 + 1 + 2 * 1 + 1 = 1 * (3 + 2)
                     col : 2 + 2 + 3 * 2 + 1 = 2 * (3 + 3)

        [i, j] -> [2 + 1 * i, 4 + 2 * j]
    */
public:
    static constexpr size_t height = 1 * (3 + static_cast<size_t>(Size));
    static constexpr size_t width  = 2 * (3 + static_cast<size_t>(Size));

    static constexpr const char empty_position_char = '+';
    static constexpr const char white_piece_char = 'O';
    static constexpr const char white_sp_piece_char = '@';
    static constexpr const char black_piece_char = 'X';
    static constexpr const char black_sp_piece_char = '*';
    static constexpr const char unknown_status_piece_char = '!';
    static constexpr const char inner_border_char = ' ';
    static constexpr const char outer_border_char = '.';
    // static constexpr const char highlight_up_down_char = '-';
    static constexpr const char highlight_left_char = '[';
    static constexpr const char highlight_right_char = ']';

    DisplayFramework() : framework_(height, std::string(width, inner_border_char)) {
        _init_framework();
        load_empty_board();
    }
    DisplayFramework(const std::vector<std::vector<std::shared_ptr<Position>>>& board_) 
        : framework_(height, std::string(width, inner_border_char)) {
        _init_framework();
        reconstruct(board_);
    } 

    void show() const {
        for (const std::string& line : framework_) {
            std::cout << line << "\n";
        }
    }
    void remove_last_sp(const Piece& last_piece) {
        remove_highlight(last_piece.row, last_piece.col);
        remove_sp(last_piece.row, last_piece.col);
    }
    void update_new_sp(Piece last_piece) {
        add_sp(last_piece);
        add_highlight(last_piece);
    }

    void reconstruct(const std::vector<std::vector<std::shared_ptr<Position>>>& board_) {
        for (int i = 0; i < static_cast<size_t>(Size); i++) {
            for (int j = 0; j < static_cast<size_t>(Size); j++) {
                switch (board_[i][j]->get_status()) {
                // Invalid = 0,
                // White   = 1,
                // WhiteSp = 2,
                // Black   = 3,
                // BlackSp = 4
                case static_cast<size_t>(Piece::Color::Invalid) : {  // empty
                    print_empty_position(i, j);
                } break;
                case static_cast<size_t>(Piece::Color::White): {
                    print_white_piece(i, j);
                } break;
                case static_cast<size_t>(Piece::Color::WhiteSp) : {
                    print_white_sp_piece(i, j);
                } break;
                case static_cast<size_t>(Piece::Color::Black) : {
                    print_black_piece(i, j);
                } break;
                case static_cast<size_t>(Piece::Color::BlackSp) : {
                    print_black_sp_piece(i, j);
                } break;
                default:
                    std::cerr << UNKNOWN_PIECE_STATUS << "\n";
                    print_unknown_status_piece(i, j);
                }
            }
        }
    }

    void load_empty_board() {
        for (int i = 0; i < static_cast<size_t>(Size); i++) {
            for (int j = 0; j < static_cast<size_t>(Size); j++) {
                print_empty_position(i, j);
            }
        }
    }

private:
    void _init_framework() {
        for (int j = 0; j < static_cast<size_t>(Size); j++) {
            framework_[0][get_col_in_framework(j)] = 'A' + j;
        }
        for (int i = 0; i < static_cast<size_t>(Size); i++) {
            framework_[get_row_in_framework(i)][0] = 'A' + i;
        }
        for (int j = 0; j <= static_cast<size_t>(Size); j++) {
            framework_[get_row_in_framework(-1)][get_col_in_framework(j)] = outer_border_char;
        }
        for (int j = 0; j <= static_cast<size_t>(Size); j++) {
            framework_[get_row_in_framework(static_cast<size_t>(Size))][get_col_in_framework(j)] = outer_border_char;
        }
        for (int i = 0; i <= static_cast<size_t>(Size); i++) {
            framework_[get_row_in_framework(i)][get_col_in_framework(-1)] = outer_border_char;
        }
        for (int i = 0; i <= static_cast<size_t>(Size); i++) {
            framework_[get_row_in_framework(i)][get_col_in_framework(static_cast<size_t>(Size))] = outer_border_char;
        }
        framework_[get_row_in_framework(-1)][get_col_in_framework(-1)] = outer_border_char;
    }

    static std::pair<size_t, size_t> get_pos_in_framework(size_t r, size_t c) {
        return {get_row_in_framework(r), get_col_in_framework(c)};
    }
    static size_t get_row_in_framework(size_t r) {
        return 2 + 1 * r;
    }
    static size_t get_col_in_framework(size_t c) {
        return 4 + 2 * c;
    }

    void print_empty_position(size_t r, size_t c) {
        framework_[get_row_in_framework(r)][get_col_in_framework(c)] = empty_position_char;
    }
    void print_white_piece(size_t r, size_t c) {
        framework_[get_row_in_framework(r)][get_col_in_framework(c)] = white_piece_char;
    }
    void print_white_sp_piece(size_t r, size_t c) {
        framework_[get_row_in_framework(r)][get_col_in_framework(c)] = white_sp_piece_char;
    }
    void print_black_piece(size_t r, size_t c) {
        framework_[get_row_in_framework(r)][get_col_in_framework(c)] = black_piece_char;
    }
    void print_black_sp_piece(size_t r, size_t c) {
        framework_[get_row_in_framework(r)][get_col_in_framework(c)] = black_sp_piece_char;
    }
    void print_unknown_status_piece(size_t r, size_t c) {
        framework_[get_row_in_framework(r)][get_col_in_framework(c)] = unknown_status_piece_char;
    }

    void remove_highlight() {
        for (std::string& line : framework_) {
            for (char& c : line) {
                if (c == highlight_left_char 
                    or c == highlight_right_char) {
                    c = inner_border_char;
                }
            }
        }
    }
    void remove_highlight(size_t r, size_t c) {
        auto [row, col] = get_pos_in_framework(r, c);
        char* ch = &framework_[row][col - 1];
        if (*ch == highlight_left_char) {
            *ch = inner_border_char;
        }
        ch = &framework_[row][col + 1];
        if (*ch == highlight_right_char) {
            *ch = inner_border_char;
        }
    }
    void add_highlight(const Piece& last_piece) {
        auto [row, col] = get_pos_in_framework(last_piece.row, 
                                               last_piece.col);
        // framework_[row - 1][col] = highlight_up_down_char;
        // framework_[row + 1][col] = highlight_up_down_char;
        framework_[row][col - 1] = highlight_left_char;
        framework_[row][col + 1] = highlight_right_char;
    }
    void add_highlight(size_t r, size_t c) {
        // assert(...)
        auto [row, col] = get_pos_in_framework(r, c);
        // framework_[row - 1][col] = highlight_up_down_char;
        // framework_[row + 1][col] = highlight_up_down_char;
        framework_[row][col - 1] = highlight_left_char;
        framework_[row][col + 1] = highlight_right_char;
    }
    void remove_sp() {
        for (auto line : framework_) {
            for (auto c : line) {
                if (c == white_sp_piece_char) {
                    c = white_piece_char;
                } else if (c == black_sp_piece_char) {
                    c = black_piece_char;
                }
            }
        }
    }
    void remove_sp(size_t r, size_t c) {
        auto [row, col] = get_pos_in_framework(r, c);
        char& ch = framework_[row][col];
        if (ch == white_sp_piece_char) {
            ch = white_piece_char;
        } else if (ch == black_sp_piece_char) {
            ch = black_piece_char;
        }
    }
    void add_sp(const Piece& last_piece) {
        auto [row, col] = get_pos_in_framework(last_piece.row, 
                                               last_piece.col);
        if (last_piece.color == Piece::Color::BlackSp) {
            framework_[row][col] = black_sp_piece_char;
        } else if (last_piece.color == Piece::Color::WhiteSp) {
            framework_[row][col] = white_sp_piece_char;
        } else {
            std::cout << "Last piece is not highlighted\n";
        }
    }

    std::vector<std::string> framework_;

};  // endof class DisplayFramework

}  // endof namespace mfwu

#endif  // __CMD_FRAMEWORK_HPP__