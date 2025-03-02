#ifndef __CHESSBOARD_HPP__
#define __CHESSBOARD_HPP__

#include "utils.hpp"


namespace mfwu {

class Player;

class ChessBoard_base {
public:
    using Color = typename Piece::Color;
    static constexpr Color Invalid = Color::Invalid;
    static constexpr Color Black = Color::Black;
    static constexpr Color White = Color::White;
    static constexpr Color BlackSp = Color::BlackSp;
    static constexpr Color WhiteSp = Color::WhiteSp;

    ChessBoard_base() {}
    virtual ~ChessBoard_base() {}
    const Piece& get_last_piece() const { return last_piece_; }
    virtual void update(const Piece& piece) = 0;
    virtual size_t size() const = 0;
    virtual size_t get_status(int row, int col) const = 0;
    virtual Command get_command() = 0;
protected:
    
    virtual void show() const = 0;
    virtual void refresh() = 0;

    virtual int count_left(const Piece&) const = 0;
    virtual int count_right(const Piece&) const = 0;
    virtual int count_up(const Piece&) const = 0;
    virtual int count_down(const Piece&) const = 0;
    virtual int count_up_left(const Piece&) const = 0;
    virtual int count_up_right(const Piece&) const = 0;
    virtual int count_down_left(const Piece&) const = 0;
    virtual int count_down_right(const Piece&) const = 0;

    Piece last_piece_;
    bool status_;
};  // endof class ChessBoard_base

template <BoardSize Size=BoardSize::Small>
class ChessBoard : public ChessBoard_base {
public:
    ChessBoard() 
        : board_(static_cast<size_t>(Size), 
            std::vector<std::unique_ptr<Postion>>(
                static_cast<size_t>(Size), nullptr)) {
        _init_board();
    }
    ChessBoard(const std::vector<std::vector<size_t>>> input_board) 
        : board_(Size, std::vector<std::unique_ptr<Postion>>(Size, nullptr)) {
        for (size_t i = 0; i < static_cast<size_t>(Size); i++) {
            for (size_t j = 0; j < static_cast<size_t>(Size); j++) {
                switch (input_board[i][j]) {
                case static_cast<size_t>(Color::Invalid) : {
                    board_[i][j] = std::make_unique<Position>(i, j);
                } break;
                case static_cast<size_t>(Color::White) : {
                    board_[i][j] = std::make_unique<Position>(Piece(i, j, Color::White));
                } break;
                case static_cast<size_t>(Color::Black) : {
                    board_[i][j] = std::make_unique<Position>(Piece(i, j, Color::Black));
                } break;
                case static_cast<size_t>(Color::BlackSp) : {
                    board_[i][j] = std::make_unique<Position>(Piece(i, j, Color::BlackSp));
                } break;
                case static_cast<size_t>(Color::WhiteSp) : {
                    board_[i][j] = std::make_unique<Position>(Piece(i, j, Color::WhiteSp));
                } break;
                default:
                    std::cout << "Position [" << i << ", " << j << "] init failed: UNKOWN PIECE COLOR\n"
                    // TODO: LOG IT
                    board_[i][j] = std::make_unique<Position>(i, j);
                }
            }
        }
    }
    ChessBoard(const ChessBoard& board) = delete;
    ChessBoard(ChessBoard&& board) = default;

    ChessBoard operator=(const ChessBoard& board) = delete;
    ChessBoard operator=(ChessBoard&& board) = default;

    ~ChessBoard() {}
    
    void reset() {
        _init_board();
    }

    void update(const Piece& piece) override {
        board_[piece.row][piece.col] 
            = std::make_unique<Position>(piece);
    }
    size_t get_status(int row, int col) const {
        assert(is_valid_row_col(row, col));
        return board_[i][j]->get_status();
    }

    std::string serialize() const {
        std::stringstream ss;
        for (size_t i = 0; i < len(); i++) {
            for (size_t j = 0; j < len(); j++) {
                ss<< board_[i][j]->get_status() << " ";
            }
            ss << "\n";
        }
        return ss.str();
    }

    size_t len() const override { return size(); }
    size_t size() const override { return static_cast<size_t>(Size); }

    int count_left(const Piece& piece) const {
        int row = piece.row;
        int col = piece.col - 1;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            col--;
        }
        return cnt;
    }
    int count_right(const Piece& piece) const {
        int row = piece.row;
        int col = piece.col + 1;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            col++;
        }
        return cnt;
    }
    int count_up(const Piece& piece) const {
        int row = piece.row - 1;
        int col = piece.col;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_row(row)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row--;
        }
        return cnt;
    }
    int count_down(const Piece& piece) const {
        int row = piece.row + 1;
        int col = piece.col;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_row(row)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row++;
        }
        return cnt;
    } 
    int count_up_left(const Piece& piece) const {
        int row = piece.row - 1;
        int col = piece.col - 1;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row--;
            col--;
        }
        return cnt;
    }
    int count_up_right(const Piece& piece) const {
        int row = piece.row - 1;
        int col = piece.col + 1;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row--;
            col++;
        }
        return cnt;
    }
    int count_down_left(const Piece& piece) const {
        int row = piece.row + 1;
        int col = piece.col - 1;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row++;
            col--;
        }
        return cnt;
    } 
    int count_down_right(const Piece& piece) const {
        int row = piece.row + 1;
        int col = piece.col + 1;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } else { break; }
            row++;
            col++;
        }
        return cnt;
    }
    int count_dir(const Piece& piece, const std::pair<int, int>& dir) const {
        int row = piece.row + dir.first;
        int col = piece.col + dir.second;
        size_t status = piece->get_status();
        int cnt = 0;
        while (is_valid_row(row) && is_valid_col(col)) {
            if (Piece::is_same_color(
                    board_[row][col]->get_status(), status)) {
                cnt++;
            } { break; }
            row += dir.first;
            col += dir.second;
        }
        return cnt;
    }
    
    struct count_res_8 {
        int right, down, left, up;
        int down_right, down_left, up_right, up_left;
    };  // endof struct count_res_8
    void count_dir(const Piece& piece, count_res_8* res) const {
        res->right = count_right(piece);
        res->down = count_down(piece);
        res->left = count_left(piece);
        res->up = count_up(piece);
        res->down_right = count_down_right(piece);
        res->down_left = count_down_left(piece);
        res->up_right = count_up_right(piece);
        res->up_left = count_up_left(piece);
    }
    struct count_res_4 {
        int left_right, up_down;
        int up_left_down_right, up_right_down_left;
    };  // endof struct count_res_4
    void count_dir(const Piece& piece, count_res_4* res) const {
        res->left_right = count_left(piece) + count_right(piece);
        res->up_down    = count_up(piece) + count_down(piece);
        res->up_left_down_right = count_up_left(piece) + count_down_right(piece);
        res->up_right_down_left = count_up_right(piece) + count_down_left(piece);
    }

protected:
    virtual void show() const = 0;
    virtual void refresh() = 0;
    virtual void show_board() const = 0;
private:
    void _init_board() {
        for (size_t i = 0; i < len(); i++) {
            for (size_t j = 0; j < len(); j++) {
                board_[i][j] = std::make_unique<Position>(i, j);
            }
        }
    } 
    
    static bool is_valid_row(int row) {  // i want them static
        return row >= 0 and row < static_cast<size_t>(Size);
    }
    static bool is_valid_col(int col) {
        return col >= 0 and col < static_cast<size_t>(Size);
    }
    static bool is_valid_row_col(int row, int col) {
        return is_valid_row(row) && is_valid_col(col);
    }

    std::vector<std::vector<std::unique_ptr<Position>>> board_;
    
};  // endof class ChessBoard

template <BoardSize Size=BoardSize::Small>
class GUIBoard : public ChessBoard<Size> {
public:
    Command get_command() override {

    }

    void show() const override {
        show_board();
    }
    void refresh() override {
        show();
    }

    using Archive_type = std::string;

};  // endof class GUIBoard

template <BoardSize Size=BoardSize::Small>
class CMDBoard : ChessBoard<Size> {
    Command get_command() override {
        std::string input_str;
        std::cin >> input_str;
        Command ret = CMDBoard::validate_input(input_str);
        if (ret.row < 0 or ret.col < 0) {
            std::cout << "invalid input, plz try again\n";
            ret = this->wait_input();
        }
        return ret;
    }
    void show() const override {
        cmd_clear();
        std::cout << CMD_HELPER << "\n";
        std::cout << INPUT_HELPER << "\n";
        show_board();
    }
    void refresh() override {
        show();
    }

private:
    static Command validate_input(const std::string& str) {
        if (str == std::string(QUIT_CMD1)
            || str == std::string(QUIT_CMD2)
            || str == std::string(QUIT_CMD3)) {
            return Command{CommandType::QUIT, {}};
        } else if (str == std::string(MENU_CMD1)
            || str == std::string(MEMU_CMD2)
            || str == std::string(MENU_CMD3)) {
            return Command{CommandType::MENU, {}};
        } else if (str == std::string(RESTART_CMD1)
            || str == std::string(RESTART_CMD2)
            || str == std::string(RESTART_CMD3)) {
            return Command{CommandType::RESTART, {}};
        }
        if (str.size() != 2) return Command{CommandType::INVALID, {}};
        return Command{CommandType::PIECE, {get_int(str[0]), get_int(str[1])}};
    }
    static int get_int(char c) {
        int res = -1
        if ('a' <= c && c <= 'z') {
            res = c - 'a';
        } else if ('A' <= c && c <= 'Z') {
            res = c - 'A';
        } else {
            return -1;
        }
        if (res >= this->size()) { return -1; }
        return res;
    }
    void show_board() const {
        // TODO: better design:
        // init a vec<vec<char> frwk first and list a set of 
        // methods to locate the position of [i, j] on the board_
        for (int i = 0; i <= static_cast<size_t>(Size); i++) {
            for (int j = 0; j <= static_cast<size_t>(Size); j++) {
                std::cout << ' .';
            }
            for (int j = 0; j <= static_cast<size_t>(Size); j++) {
                std::cout << '.';
                switch (board_[i][j]->get_status()) {
                // Invalid = 0,
                // White   = 1,
                // WhiteSp = 2,
                // Black   = 3,
                // BlackSp = 4
                case static_cast<size_t>(Piece::Color::Invalid) : {  // empty
                    print_empty_position();
                } break;
                case static_cast<size_t>(Piece::Color::White): {
                    print_white_piece();
                } break;
                case static_cast<size_t>(Piece::Color::WhiteSp) : {
                    print_white_sp_piece();
                } break;
                case static_cast<size_t>(Piece::Color::Black) : {
                    print_black_piece();
                } break;
                case static_cast<size_t>(Piece::Color::BlackSp) : {
                    print_black_sp_piece();
                } break;
                default:
                    std::cerr << UNKNOWN_PIECE_STATUS << "\n";
                    print_unknown_status_piece();
                }
            }
            std::cout << ".  " << char('A' + i) << "\n";
        }
    }
    static void print_empty_position() {
        std::cout << empty_position_char;
    }
    static void print_white_piece() {
        std::cout << white_piece_char;
    }
    static void print_white_sp_piece() {
        std::cout << white_sp_piece_char;
    }
    static void print_black_piece() {
        std::cout << black_piece_char;
    }
    static void print_black_sp_piece() {
        std::cout << black_sp_piece_char;
    }
    static void print_unknown_status_piece() {
        std::cout << unknown_status_piece_char;
    }
    constexpr char empty_position_char = '+';
    constexpr char white_piece_char = 'O';
    constexpr char wihte_sp_piece_char = '@';
    constexpr char black_piece_char = 'X';
    constexpr char black_sp_piece_char = '*';
    constexpr char unknown_status_piece_char = '!'
};  // endof class CMDBoard

}  // endof namespace mfwu

#endif  // __CHESSBOARD_HPP__