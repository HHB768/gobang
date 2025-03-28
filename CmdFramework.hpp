#ifndef __CMD_FRAMEWORK_HPP__
#define __CMD_FRAMEWORK_HPP__

#include "common.hpp"
#include "Logger.hpp"

// gui(?) of cmd mode lol
namespace mfwu {

template <BoardSize Size=BoardSize::Small>
class DisplayFramework {
    /*  
        board: 2 * 3
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
    static constexpr size_t size_   = static_cast<size_t>(Size);
    static constexpr size_t height_ = 1 * (3 + size_);
    static constexpr size_t width_  = 2 * (3 + size_);

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

    DisplayFramework() : framework_(height_, std::string(width_, inner_border_char)) {
        _init_framework();
        load_empty_board();
    }
    DisplayFramework(const std::vector<std::vector<std::shared_ptr<Position>>>& board_) 
        : framework_(height_, std::string(width_, inner_border_char)) {
        _init_framework();
        reconstruct(board_);
    } 

    void show() const {
        std::stringstream ss;
        log_debug("board: ");
        for (const std::string& line : framework_) {
            ss << line << "\n";
            log_debug(XQ4GB_TIMESTAMP, line.c_str());
        }
        std::cout << ss.str();
    }
    void show_without_log() const {
        std::stringstream ss;
        for (const std::string& line : framework_) {
            ss << line << "\n";
        }
        std::cout << ss.str();
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
        for (int i = 0; i < size_; i++) {
            for (int j = 0; j < size_; j++) {
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
                    logerr_unknown_piece_status();
                    print_unknown_status_piece(i, j);
                }
            }
        }
    }

    void load_empty_board() {
        for (int i = 0; i < size_; i++) {
            for (int j = 0; j < size_; j++) {
                print_empty_position(i, j);
            }
        }
        remove_highlight();
    }
    void set_highlight(int r, int c) {
        add_highlight(r, c);
    }

private:
    void _init_framework() {
        for (int j = 0; j < size_; j++) {
            framework_[0][get_col_in_framework(j)] = 'A' + j;
        }
        for (int i = 0; i < size_; i++) {
            framework_[get_row_in_framework(i)][0] = 'A' + i;
        }
        for (int j = 0; j <= size_; j++) {
            // framework_[get_row_in_framework(-1)][get_col_in_framework(j)] = outer_border_char;
            get_pos_ref_in_framework(-1, j) = outer_border_char;
        }
        for (int j = 0; j <= size_; j++) {
            // framework_[get_row_in_framework(size_)][get_col_in_framework(j)] = outer_border_char;
            get_pos_ref_in_framework((int)size_, j) = outer_border_char;
        }
        for (int i = 0; i <= size_; i++) {
            // framework_[get_row_in_framework(i)][get_col_in_framework(-1)] = outer_border_char;
            get_pos_ref_in_framework(i, -1) = outer_border_char;
        }
        for (int i = 0; i <= size_; i++) {
            // framework_[get_row_in_framework(i)][get_col_in_framework(size_)] = outer_border_char;
            get_pos_ref_in_framework(i, (int)size_) = outer_border_char;
        }
        // framework_[get_row_in_framework(-1)][get_col_in_framework(-1)] = outer_border_char;
        get_pos_ref_in_framework(-1, -1) = outer_border_char;
    }
    // i change size_t into int on 03.22
    // but i havent found init problem before
    char& get_pos_ref_in_framework(int r, int c) {
        return framework_[get_row_in_framework(r)][get_col_in_framework(c)];
    }
    static std::pair<size_t, size_t> get_pos_in_framework(int r, int c) {
        return {get_row_in_framework(r), get_col_in_framework(c)};
    }
    static size_t get_row_in_framework(int r) {
        return 2 + 1 * r;
    }
    static size_t get_col_in_framework(int c) {
        return 4 + 2 * c;
    }

    void print_empty_position(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = empty_position_char;
        get_pos_ref_in_framework(r, c) = empty_position_char;
    }
    void print_white_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = white_piece_char;
        get_pos_ref_in_framework(r, c) = white_piece_char;
    }
    void print_white_sp_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = white_sp_piece_char;
        get_pos_ref_in_framework(r, c) = white_sp_piece_char;
    }
    void print_black_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = black_piece_char;
        get_pos_ref_in_framework(r, c) = black_piece_char;
    }
    void print_black_sp_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = black_sp_piece_char;
        get_pos_ref_in_framework(r, c) = black_sp_piece_char;
    }
    void print_unknown_status_piece(int r, int c) {
        // framework_[get_row_in_framework(r)][get_col_in_framework(c)] = unknown_status_piece_char;
        get_pos_ref_in_framework(r, c) = unknown_status_piece_char;
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
    void remove_highlight(int r, int c) {
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
    void add_highlight(int r, int c) {
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
    void remove_sp(int r, int c) {
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
            log_warn("Last piece is not colored as sp");
            log_warn(XQ4GB_TIMESTAMP, "trying to fix it with real color...");
            if (last_piece.get_status() == 0) {
                log_error("last piece is not placed yet");
            } else {
                add_sp(Piece{last_piece.row, last_piece.col, 
                       Piece::Color{Piece::get_real_color(last_piece.color) + 1}});  // CHECK: can i?
            }
        }
    }

    std::vector<std::string> framework_;

};  // endof class DisplayFramework

}  // endof namespace mfwu

#endif  // __CMD_FRAMEWORK_HPP__