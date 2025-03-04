#ifndef __CHESSBOARD_HPP__
#define __CHESSBOARD_HPP__

#include "utils.hpp"


namespace mfwu {

// class Player;

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

    virtual void reset() = 0;
    const Piece& get_last_piece() const { return last_piece_; }
    virtual void update(const Piece& piece) = 0;
    virtual size_t size() const = 0;
    virtual size_t get_status(int row, int col) const = 0;
    virtual Command get_command() = 0;

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

    virtual int count_dir(const Piece& piece, const std::pair<int, int>& dir) const = 0;
    virtual void count_dir(const Piece& piece, count_res_8* res) const = 0;
    virtual void count_dir(const Piece& piece, count_res_4* res) const = 0;

protected:
    Piece last_piece_;
    bool status_;
};  // endof class ChessBoard_base

template <BoardSize Size=BoardSize::Small>
class ChessBoard : public ChessBoard_base {
public:
    static constexpr size_t size_ = static_cast<size_t>(Size);
    ChessBoard() 
        : board_(static_cast<size_t>(Size), 
            std::vector<std::shared_ptr<Position>>(
                static_cast<size_t>(Size))) {
        _init_board();
    }
    ChessBoard(const std::vector<std::vector<size_t>>& input_board) 
        : board_(Size, std::vector<std::shared_ptr<Position>>(Size)) {
        for (size_t i = 0; i < static_cast<size_t>(Size); i++) {
            for (size_t j = 0; j < static_cast<size_t>(Size); j++) {
                switch (input_board[i][j]) {
                case static_cast<size_t>(Color::Invalid) : {
                    board_[i][j] = std::make_shared<Position>(i, j);
                } break;
                case static_cast<size_t>(Color::White) : {
                    board_[i][j] = std::make_shared<Position>(Piece(i, j, Color::White));
                } break;
                case static_cast<size_t>(Color::Black) : {
                    board_[i][j] = std::make_shared<Position>(Piece(i, j, Color::Black));
                } break;
                case static_cast<size_t>(Color::BlackSp) : {
                    board_[i][j] = std::make_shared<Position>(Piece(i, j, Color::BlackSp));
                } break;
                case static_cast<size_t>(Color::WhiteSp) : {
                    board_[i][j] = std::make_shared<Position>(Piece(i, j, Color::WhiteSp));
                } break;
                default:
                    std::cout << "Position [" << i << ", " << j << "] init failed: UNKOWN PIECE COLOR\n";
                    // TODO: LOG IT
                    board_[i][j] = std::make_shared<Position>(i, j);
                }
            }
        }
    }
    ChessBoard(const ChessBoard& board) = default;
    ChessBoard(ChessBoard&& board) = default;

    ChessBoard& operator=(const ChessBoard& board) = default;
    ChessBoard& operator=(ChessBoard&& board) = default;

    virtual ~ChessBoard() {}
    
    void reset() override {
        _init_board();
    }

    virtual void update(const Piece& piece) override {
        board_[last_piece_.row][last_piece_.col] 
            = std::make_shared<Position>(Piece{last_piece_.row, last_piece_.col, 
                                               Piece::Color{last_piece_.get_status() - 1}});
        board_[piece.row][piece.col] 
            = std::make_shared<Position>(piece);
    }
    size_t get_status(int row, int col) const {
        assert(is_valid_row_col(row, col));
        return board_[row][col]->get_status();
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

    size_t len() const { return size(); }
    size_t size() const override { return static_cast<size_t>(Size); }

    int count_left(const Piece& piece) const override {
        int row = piece.row;
        int col = piece.col - 1;
        size_t status = piece.get_status();
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
    int count_right(const Piece& piece) const override {
        int row = piece.row;
        int col = piece.col + 1;
        size_t status = piece.get_status();
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
    int count_up(const Piece& piece) const override {
        int row = piece.row - 1;
        int col = piece.col;
        size_t status = piece.get_status();
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
    int count_down(const Piece& piece) const override {
        int row = piece.row + 1;
        int col = piece.col;
        size_t status = piece.get_status();
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
    int count_up_left(const Piece& piece) const override {
        int row = piece.row - 1;
        int col = piece.col - 1;
        size_t status = piece.get_status();
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
    int count_up_right(const Piece& piece) const override {
        int row = piece.row - 1;
        int col = piece.col + 1;
        size_t status = piece.get_status();
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
    int count_down_left(const Piece& piece) const override {
        int row = piece.row + 1;
        int col = piece.col - 1;
        size_t status = piece.get_status();
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
    int count_down_right(const Piece& piece) const override {
        int row = piece.row + 1;
        int col = piece.col + 1;
        size_t status = piece.get_status();
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
    int count_dir(const Piece& piece, const std::pair<int, int>& dir) const override {
        int row = piece.row + dir.first;
        int col = piece.col + dir.second;
        size_t status = piece.get_status();
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
    void count_dir(const Piece& piece, count_res_8* res) const override {
        res->right = count_right(piece);
        res->down = count_down(piece);
        res->left = count_left(piece);
        res->up = count_up(piece);
        res->down_right = count_down_right(piece);
        res->down_left = count_down_left(piece);
        res->up_right = count_up_right(piece);
        res->up_left = count_up_left(piece);
    }
    void count_dir(const Piece& piece, count_res_4* res) const override {
        res->left_right = count_left(piece) + count_right(piece);
        res->up_down    = count_up(piece) + count_down(piece);
        res->up_left_down_right = count_up_left(piece) + count_down_right(piece);
        res->up_right_down_left = count_up_right(piece) + count_down_left(piece);
    }

    virtual void show() const = 0;
    virtual void refresh() = 0;
protected:
    virtual void show_board() const = 0;
    std::vector<std::vector<std::shared_ptr<Position>>> board_;
private:
    void _init_board() {
        for (size_t i = 0; i < len(); i++) {
            for (size_t j = 0; j < len(); j++) {
                board_[i][j] = std::make_shared<Position>(i, j);
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
};  // endof class ChessBoard

// template <BoardSize Size=BoardSize::Small>
// class GUIBoard : public ChessBoard<Size> {
// public:
//     Command get_command() override {
//         return Command{CommandType::PIECE, {0, 0}};
//     }

//     void show() const override {

//     }
//     void refresh() override {
//         show();
//     }

//     using Archive_type = std::string;

// };  // endof class GUIBoard

template <BoardSize Size=BoardSize::Small>
class CMDBoard : public ChessBoard<Size> {
public:
    static constexpr size_t frwk_size = 2 * (3 + static_cast<size_t>(Size));
    CMDBoard() 
        : ChessBoard<Size>(), framework_(frwk_size, std::string(frwk_size, inner_border_char)) {
        _init_framework();
    }
    CMDBoard(const std::vector<std::vector<size_t>>& input_board) 
        : ChessBoard<Size>(input_board), framework_(frwk_size, std::string(frwk_size, inner_border_char)) {
        _init_framework();
        reconstruct_framework();
    }
    CMDBoard(const CMDBoard& board) = default;  // really work?
    CMDBoard(CMDBoard&& board) = default;

    CMDBoard& operator=(const CMDBoard& board) = default;
    CMDBoard& operator=(CMDBoard&& board) = default;

    void update(const Piece& piece) override {
        ChessBoard<Size>::update(piece);
        update_framework();
    }

    Command get_command() override {
        std::string input_str;
        std::cin >> input_str;
        Command ret = CMDBoard::validate_input(input_str);
        if (ret.pos.row < 0 or ret.pos.col < 0) {
            std::cout << "invalid input, plz try again\n";
            ret = this->get_command();
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
            || str == std::string(MENU_CMD2)
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
        int res = -1;
        if ('a' <= c && c <= 'z') {
            res = c - 'a';
        } else if ('A' <= c && c <= 'Z') {
            res = c - 'A';
        } else {
            return -1;
        }
        if (res >= static_cast<size_t>(Size)) { return -1; }
        return res;
    }
    void show_board() const {
        // TODO: better design:
        // init a vec<vec<char> frwk first and list a set of 
        // methods to locate the position of [i, j] on the board_  25.03.03
        // TODO: better design:
        // make framework a class
        for (const std::string& line : framework_) {
            std::cout << line << "\n";
        }
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
    static constexpr const char empty_position_char = '+';
    static constexpr const char white_piece_char = 'O';
    static constexpr const char white_sp_piece_char = '@';
    static constexpr const char black_piece_char = 'X';
    static constexpr const char black_sp_piece_char = '*';
    static constexpr const char unknown_status_piece_char = '!';
    static constexpr const char inner_border_char = ' ';
    static constexpr const char outer_border_char = '.';
    static constexpr const char highlight_up_down_char = '-';
    static constexpr const char highlight_left_right_char = '|';

    std::vector<std::string> framework_;
    //  board: 2 * 3
    /*
            A B C 
          . . . . .
        A . O + X .
        B . + O + .
          . . . . .

        framework:   row : 2 + 2 + 2 * 2 + 2 = 2 * (3 + 2)
                     col : 2 + 2 + 3 * 2 + 2 = 2 * (3 + 3)

        [i, j] -> [4 + 2 * i, 4 + 2 * j]
    */
    static std::pair<size_t, size_t> get_pos_in_framework(size_t r, size_t c) {
        return {get_row_in_framework(r), get_col_in_framework(c)};
    }
    static size_t get_row_in_framework(size_t r) {
        return 4 + 2 * r;
    }
    static size_t get_col_in_framework(size_t c) {
        return 4 + 2 * c;
    }
    void remove_highlight() {
        for (std::string& line : framework_) {
            for (char& c : line) {
                if (c == highlight_up_down_char 
                    or c == highlight_left_right_char) {
                    c = inner_border_char;
                }
            }
        }
    }
    void add_highlight() {
        auto [row, col] = get_pos_in_framework(this->last_piece_.row, 
                                               this->last_piece_.col);
        framework_[row - 1][col] = highlight_up_down_char;
        framework_[row + 1][col] = highlight_up_down_char;
        framework_[row][col - 1] = highlight_left_right_char;
        framework_[row][col + 1] = highlight_left_right_char;
    }
    void add_highlight(size_t r, size_t c) {
        // assert(...)
        auto [row, col] = get_pos_in_framework(r, c);
        framework_[row - 1][col] = highlight_up_down_char;
        framework_[row + 1][col] = highlight_up_down_char;
        framework_[row][col - 1] = highlight_left_right_char;
        framework_[row][col + 1] = highlight_left_right_char;
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
    void add_sp() {
        auto [row, col] = get_pos_in_framework(this->last_piece_.row, 
                                               this->last_piece_.col);
        if (this->last_piece_.color == Piece::Color::BlackSp) {
            framework_[row][col] = black_sp_piece_char;
        } else if (this->last_piece_.color == Piece::Color::WhiteSp) {
            framework_[row][col] = white_sp_piece_char;
        } else {
            std::cout << "Last piece is not highlighted\n";
        }
    }
    void update_framework() {
        remove_highlight(); add_highlight();
        remove_sp(); add_sp();
    }
    void _init_framework() {
        for (int i = 0; i <= static_cast<size_t>(Size); i++) {
            framework_[0][get_col_in_framework(i)] = 'A' + i;
        }
        for (int j = 0; j <= static_cast<size_t>(Size); j++) {
            framework_[get_row_in_framework(j)][0] = 'A' + j;
        }
        for (int i = 0; i <= static_cast<size_t>(Size); i++) {
            framework_[get_row_in_framework(-1)][get_col_in_framework(i)] = 'A' + i;
        }
        for (int i = 0; i <= static_cast<size_t>(Size); i++) {
            framework_[get_row_in_framework(static_cast<size_t>(Size))][get_col_in_framework(i)] = 'A' + i;
        }
        for (int j = 0; j <= static_cast<size_t>(Size); j++) {
            framework_[get_row_in_framework(j)][get_row_in_framework(-1)] = 'A' + j;
        }
        for (int j = 0; j <= static_cast<size_t>(Size); j++) {
            framework_[get_row_in_framework(j)][get_row_in_framework(static_cast<size_t>(Size))] = 'A' + j;
        }
    }
    void reconstruct_framework() {
        for (int i = 0; i <= static_cast<size_t>(Size); i++) {
            for (int j = 0; j <= static_cast<size_t>(Size); j++) {
                switch (this->board_[i][j]->get_status()) {
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
    
};  // endof class CMDBoard

}  // endof namespace mfwu

#endif  // __CHESSBOARD_HPP__