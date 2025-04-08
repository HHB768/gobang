#ifndef __ROBOTPLAYER_HPP__
#define __ROBOTPLAYER_HPP__

#include "Player.hpp"

namespace mfwu {

class DeductionBoard_base {
public:
    DeductionBoard_base() = delete;
    DeductionBoard_base(const std::vector<std::vector<size_t>>& board) 
        : board_(board) {}
    DeductionBoard_base(std::vector<std::vector<size_t>>&& board) 
        : board_(std::move(board)) {}

    std::vector<size_t>& operator[](int idx) {
        return board_[idx];
    }
    virtual size_t size() const = 0;
    virtual void set_piece(const Piece& p) = 0;
    virtual void reset_pos(const Position& p) = 0;

protected:
    std::vector<std::vector<size_t>> board_;
};  // endof class DeductionBoard_base

template <BoardSize Size=BoardSize::Small>
class DeductionBoard : public DeductionBoard_base {
public:
    using base_type = DeductionBoard_base;   

    DeductionBoard() = delete;
    DeductionBoard(const std::vector<std::vector<size_t>>& board) 
        : base_type(board), board_log_(board) {}
    DeductionBoard(std::vector<std::vector<size_t>>&& board)
        : base_type(std::move(board)), board_log_(this->board) {}

    size_t size() const override { return static_cast<size_t>(Size); }
    void deduce_new_piece(const Piece& p, int depth) override {
        size_t real_status = Piece::get_real_status(p.color);
        this->board_[p.row][p.col] = real_status + 1;
        board_log_.update(p.row, p.col, real_status + 1);
        board_log_.show_infer(depth);
    }
    void deduce_reset_piece(const Position& p) override {
        this->board_[p.row][p.col] = 0;
        board_log_.update(p.row, p.col, 0);
    }
private:
    InferDisplayer<Size> board_log_;
};  // endof class DeductionBoard

class RobotPlayer : public Player {
public:
    RobotPlayer() : Player() {}
    RobotPlayer(std::shared_ptr<ChessBoard_base> board, Piece::Color color) : Player(board, color) {}

    virtual CommandType play() override {
        Position pos = this->get_best_position();
        if (pos.row < 0 || pos.col < 0) {
            log_info("Robot's pos: [%d, %d], an ending may have been met");
            return CommandType::INVALID;
        } // else
        log_info("Robot's pos: [%d, %d]", pos.row, pos.col);
        this->place(pos);
        return CommandType::PIECE;
    }

    void place(const Position& pos) override {
        Player::place(pos);
    }

protected:
    virtual Position get_best_position() const = 0;
};  // endof class RobotPlayer

class DebugRobot : public RobotPlayer {
public:
    DebugRobot() : RobotPlayer() {}
    DebugRobot(std::shared_ptr<ChessBoard_base> board, Piece::Color color) : RobotPlayer(board, color) {}
private:
    Position get_best_position() const override {
        size_t len = this->board_->size();
        int row = -1, col = -1;
        while (is_valid(row, col) == false) {
            row = rand() % len;
            col = rand() % len;
        }
        sleep(1);
        return {row, col};
    }

    bool is_valid(int row, int col) const {
        if (row < 0 || row >= this->board_->size()) return false;
        if (col < 0 || col >= this->board_->size()) return false;
        if (this->board_->get_status(row, col) != 0) return false;
        return true;
    }
};  // endof class DebugRobot;

class DummyRobot : public RobotPlayer {
public:
    DummyRobot() : RobotPlayer()/*, scores_(this->board_->size(), std::vector<float>(this->board_->size(), 0.0F)) */{}
    DummyRobot(std::shared_ptr<ChessBoard_base> board, Piece::Color color) : RobotPlayer(board, color)/*,
        scores_(board->size(), std::vector<float>(board->size(), 0.0F))*/ {}
    ~DummyRobot() {}
private:
    Position get_best_position() const override {
        int row = -1, col = -1;
        float hi_score = -1;
        size_t sz = this->board_->size();
        bool all_clear_flag = true;
        for (int i = 0; i < sz; i++) {
            for (int j = 0; j < sz; j++) {
                if (this->board_->get_status(i, j) == 0) {
                    float score = calc_pos(i, j);
                    if (score > hi_score) {
                        row = i, col = j;
                        hi_score = score;
                    }
                } else {
                    all_clear_flag = false;
                }
            }
        }
        if (all_clear_flag) 
        return {(int)sz / 2, (int)sz / 2};
        return {row, col};
    }

    float calc_pos(int row, int col) const {
        count_res_4 cnt;
        this->board_->count_dir(Piece{row, col, this->player_color_}, &cnt);
        float res = calc_dir(cnt.left_right) + calc_dir(cnt.up_down)
                    + calc_dir(cnt.up_left_down_right)
                    + calc_dir(cnt.up_right_down_left);
        // scores_[row][col] = res;
        return res;
    }

    float calc_dir(int cnt) const {
        assert(cnt >= 0);
        switch (cnt + 1) {
        case 1:
            return s_1iar;
        case 2:
            return s_2iar;
        case 3:
            return s_3iar;
        case 4:
            return s_4iar;
        case 5:
            return s_5iar;
        default:
            return s_5iar;
        }
    }

    static constexpr int s_5iar = 10000;
    static constexpr int s_4iar = 1000;
    static constexpr int s_3iar = 200;
    static constexpr int s_2iar = 80;
    static constexpr int s_1iar = 10;

    // bool is_valid(int row, int col) const {
    //     if (row < 0 || row >= this->board_->size()) return false;
    //     if (col < 0 || col >= this->board_->size()) return false;
    //     if (this->board_->get_status(row, col) != 0) return false;
    //     return true;
    // }

    // mutable std::vector<std::vector<float>> scores_;
};  // endof class DummyRobot

class HumanLikeRobot : public RobotPlayer {
public:
    HumanLikeRobot() : RobotPlayer() {}
    HumanLikeRobot(std::shared_ptr<ChessBoard_base> board, Piece::Color color) : RobotPlayer(board, color) {}
    ~HumanLikeRobot() {}
private:
    Position get_best_position() const override {
        size_t sz = this->board_->size();
        
        bool is_clear_flag = true; // TODO: board_->is_clear
        for (int i = 0; i < sz; i++) {
            for (int j = 0; j < sz; j++) {
                if (this->board_->get_status(i, j)) {
                    is_clear_flag = false;
                }
            }
        }
        if (is_clear_flag) return {(int)sz / 2, (int)sz / 2};

        // std::shared_ptr<DeductionBoard_base> deduction_board;
        // TODO: deduction board should update with last_piece
        // just like framework does...  X 25.04.08
        // then move this part to constructor and rm mutable qualifier
        switch (this->board_->size()) {
        case static_cast<size_t>(BoardSize::Small) : {
            deduction_board_ = std::make_shared<DeductionBoard<BoardSize::Small>>(this->board_->snap());
        } break;
        case static_cast<size_t>(BoardSize::Middle) : {
            deduction_board_ = std::make_shared<DeductionBoard<BoardSize::Middle>>(this->board_->snap());
        } break;
        case static_cast<size_t>(BoardSize::Large) : {
            deduction_board_ = std::make_shared<DeductionBoard<BoardSize::Large>>(this->board_->snap());
        } break;
        default:
            log_error("Deduction board is not correctly created");
            log_error("bcz the size is: %lu", this->board_->size());
        }
        auto [_, best_row, best_col] = get_best(INFERENCE_DEPTH, this->player_color_);
        return {best_row, best_col};
    }
    struct cmp {
        bool operator()(const std::tuple<float, int, int>& a, const std::tuple<float, int, int>& b) const {
            return std::get<0>(a) > std::get<0>(b);
        }
    };  // endof struct cmp
    std::tuple<float, int, int> get_best(int depth, Piece::Color color) const {
        // if (depth == 0) return get_best(color);
        std::priority_queue<std::tuple<float, int, int>, std::vector<std::tuple<float, int, int>>, cmp> pq;
        int num_of_choices = 3;  // 3
        size_t sz = deduction_board_->size();
        assert(deduction_board_->size() > 0 && deduction_board_->size() == (*deduction_board_)[0].size());
        std::vector<std::vector<float>> score_board(sz, std::vector<float>(sz, 0.0F));  
        // 搞一个score_board把结果存下来的意义在哪呢：debug很好用:D
        
        // 先筛选出最有价值的三个点，后面再详细看
        for (int row = 0; row < sz; row++) {
            for (int col = 0; col < sz; col++) {
                if ((*deduction_board_)[row][col] != 0) { continue; }  // only search empty pos
                score_board[row][col] += calc_pos(row, col, color)
                    + 0.6 * calc_pos(row, col, Piece::Color{Piece::get_op_real_status(color)});
                if (pq.size() < num_of_choices || score_board[row][col] - std::get<0>(pq.top()) > 0 - eps) {
                    while (pq.size() >= num_of_choices) {
                        pq.pop();
                    }
                    pq.emplace(score_board[row][col], row, col);
                }
            }
        }
        if (pq.empty()) { return {0, -1, -1}; }
        auto [max_score, best_row, best_col] = pq.top();
        // float max_score = INT_MIN / 2;
        // int best_row = -1, best_col = -1;
        int best_score_num = 1;
        while (!pq.empty()) {
            auto [now_score, row, col] = pq.top();
            pq.pop();
            log_infer_pq_top_pos(depth, row, col, now_score);
            if (depth <= 0) {
                log_infer_max_depth(depth);
            } else {
                log_infer_this_move(depth, color, row, col);
                deduction_board_->set_piece(Piece{row, col, color});
                auto [op_score, op_row, op_col] = get_best(depth - 1, Piece::Color{Piece::get_op_real_status(color)});  // 不应该反向吗？就像这样
                if (op_row < 0 || op_col < 0) {
                    deduction_board_->reset_pos(Position{row, col});
                    continue;
                }
                log_infer_op_move(depth, color, op_row, op_col);
                deduction_board_->set_piece(Piece{op_row, op_col, Piece::Color{Piece::get_op_real_status(color)}});
                auto [_, next_row, next_col] = get_best(depth - 1, color);
                if (next_row < 0 || next_col < 0) {
                    deduction_board_->reset_pos(Position{row, col});
                    deduction_board_->reset_pos(Position{op_row, op_col});
                    continue;
                }
                log_infer_next_move(depth, color, next_row, next_col);
                deduction_board_->set_piece(Piece{next_row, next_col, color});
                float next_eval = calc_pos(row, col, color)
                    + 0.6 * calc_pos(row, col, Piece::Color{Piece::get_op_real_status(color)});
                now_score += 0.2 * next_eval;
                deduction_board_->reset_pos(Position{row, col});
                deduction_board_->reset_pos(Position{op_row, op_col});
                deduction_board_->reset_pos(Position{next_row, next_col});
            }
            
            if (std::fabs(now_score - max_score) <= eps) {
                best_score_num++;
                if (rand() % best_score_num < 1) {
                    best_row = row;
                    best_col = col;
                }
            } else if (now_score > max_score) {
                best_score_num = 1;
                max_score = now_score;
                best_row = row;
                best_col = col;
            } else {
                // score < max_score
            }
        }
        return {max_score, best_row, best_col};
    }
    float calc_pos(int row, int col, Piece::Color color) const {
        std::vector<int> res;
        res.reserve(4);
        for (auto&& [inc_r, inc_c] : half_dirs) {
            res.emplace_back(search_dir_rank(row, col, inc_r, inc_c, color));
        }
        std::sort(res.begin(), res.end());

        if (res[0] == 0) return score_map[0];
        else if (res[0] == 1) return score_map[1];
        else if (res[0] == 2 && res[1] == 2) return score_map[1] * 0.1;
        else if (res[0] == 2 && res[1] == 3) return score_map[2] * 1.5;
        return 0.8 * score_map[res[0]] + 0.2 * score_map[res[1]];
    }

    int search_dir_rank(int row, int col, int inc_r, int inc_c, Piece::Color color) const {
        assert(this->board_->get_status(row, col) == 0);
        int seq = 1, emp = 0, jump1 = 0, jump2 = 0;
        
        search_one_dir(row, col, inc_r,  inc_c, seq, emp, jump1, color);
        search_one_dir(row, col, -inc_r, -inc_c, seq, emp, jump2, color);  // bugfix
        
        int rank = 0;
        if (seq >= 5) {
            rank = 0;
        } else if (emp == 0 || jump1 >= 4 || jump2 >= 4) {
            rank = 7;
        } else if (seq == 4 && emp == 2 || seq == 3 && emp == 2 && jump1 && jump2
                   || seq == 2 && jump1 >= 2 && jump2 >= 2 || seq == 1 && jump1 == 3 && jump2 == 3) {
            rank = 1;
        } else if (seq == 4 && emp == 1 || seq == 3 && (emp + jump1 + jump2 >= 2)
                   || seq == 2 && (emp == 2 && (jump1 + jump2) >= 2   // really?
                                || emp == 1 && (jump1 + jump2) >= 3)
                   || seq == 1 && (jump1 == 3 || jump2 == 3)) {
            rank = 2;
        } else if (seq == 2 && emp == 2 && (jump1 || jump2) 
                   || seq == 1 && emp == 2 && (jump1 == 2 || jump2 == 2)) {  // 这个t级有待研究
            rank = 3;
        } else if (seq == 3 || seq == 2 && (emp + jump1 + jump2) >= 2 
                            || seq == 1 && (emp + jump1 + jump2) >= 3) {
            rank = 4;
        } else if (seq == 2 || seq == 1 && (emp + jump1 + jump2 >= 2)) {
            rank = 5;
        } else if (seq == 1) {
            rank = 6;
        } else {
            assert(1 == 0);
            log_error("lol upi forget this situation");
            log_error(XQ4GB_TIMESTAMP, "↑ you -> upi");
        }
        return rank;
    }
    void search_one_dir(int row, int col, int inc_r, int inc_c, 
                        int& seq, int& emp, int& jump, Piece::Color color) const {
        // size_t sz = this->board_->size();
        for (int step = 1; step < 5; step++) {
            int cur_r = row + step * inc_r;
            int cur_c = col + step * inc_c;
            if (!this->board_->is_valid_pos(cur_r, cur_c)) { break; }
            if (this->board_->get_status(cur_r, cur_c) == 0) {
                emp++;
                for (int inc_step = 1; inc_step < 5; inc_step++) {
                    cur_r = row + (step + inc_step) * inc_r;
                    cur_c = col + (step + inc_step) * inc_c;
                    if (!this->board_->is_valid_pos(cur_r, cur_c)) { break; }
                    if (Piece::get_real_status(this->board_->get_status(cur_r, cur_c))
                        == Piece::get_real_status(color)) {
                        jump++;
                    } else { break; }
                }
                break;
            } else if (Piece::get_real_status(this->board_->get_status(cur_r, cur_c))
                       == Piece::get_real_status(color)) {
                seq++;
            } else {
                break;
            }
        }
    }
    static void log_infer_pq_top_pos(size_t depth, int row, int col, float now_score) {
#ifndef __LOG_INFERENCE_ELSEWHERE__
        log_infer(depth, "Prior pos: [%d, %d], score: %.2f", row, col, now_score);
#else  // __LOG_INFERENCE_ELSEWHERE__
        log_infer(depth, "- %d %d %.2f", row, col, now_score);
#endif // __LOG_INFERENCE_ELSEWHERE__
    }
    static void log_infer_max_depth(size_t depth) {
#ifndef __LOG_INFERENCE_ELSEWHERE__
        log_infer(XQ4GB_TIMESTAMP, depth, "max depth met");
#else  // __LOG_INFERENCE_ELSEWHERE__
        log_infer(depth, "!");
#endif // __LOG_INFERENCE_ELSEWHERE__
    }
    static void log_infer_this_move(size_t depth, Piece::Color color, int row, int col) {
#ifndef __LOG_INFERENCE_ELSEWHERE__
        log_infer(depth, "[1] infering %s player's optional pos: [%d, %d]",
                  Piece::get_real_status(color) == 
                  static_cast<size_t>(Piece::Color::Black) ? "black" : "white", 
                  row, col);
#else  // __LOG_INFERENCE_ELSEWHERE__
        log_infer(depth, "1 %s %d %d", Piece::get_real_status(color) == 
                  static_cast<size_t>(Piece::Color::Black) ? "b" : "w", 
                  row, col);
#endif // __LOG_INFERENCE_ELSEWHERE__
    }
    static void log_infer_op_move(size_t depth, Piece::Color color, int op_row, int op_col) {
#ifndef __LOG_INFERENCE_ELSEWHERE__
        log_infer(depth, "[2] infering %s player's optional pos: [%d, %d]",
                  Piece::get_op_real_status(color) == 
                  static_cast<size_t>(Piece::Color::Black) ? "black" : "white", 
                  op_row, op_col);
#else  // __LOG_INFERENCE_ELSEWHERE__
        log_infer(depth, "2 %s %d %d", Piece::get_op_real_status(color) == 
                  static_cast<size_t>(Piece::Color::Black) ? "b" : "w", 
                  op_row, op_col);
#endif // __LOG_INFERENCE_ELSEWHERE__
    }
    static void log_infer_next_move(size_t depth, Piece::Color color, int next_row, int next_col) {
#ifndef __LOG_INFERENCE_ELSEWHERE__
        log_infer(depth, "[3] infering %s player's optional pos: [%d, %d]",
                  Piece::get_real_status(color) == 
                  static_cast<size_t>(Piece::Color::Black) ? "black" : "white", 
                  next_row, next_col);
#else // __LOG_INFERENCE_ELSEWHERE__
        log_infer(depth, "3 %s %d %d", Piece::get_real_status(color) == 
                  static_cast<size_t>(Piece::Color::Black) ? "b" : "w", 
                  next_row, next_col);
#endif // __LOG_INFERENCE_ELSEWHERE__
    }
    
    mutable std::shared_ptr<DeductionBoard_base> deduction_board_;
                    
};  // endof class HumanLikeRobot

class SmartRobot : public RobotPlayer {
public:
    SmartRobot() : RobotPlayer() {}
    SmartRobot(std::shared_ptr<ChessBoard_base> board, Piece::Color color) : RobotPlayer(board, color) {}
    ~SmartRobot() {}

private:
    Position get_best_position() const override {

        return {};
    }
};  // enof class SmartRobot


}  // endof namespace mfwu

#endif // __ROBOTPLAYER_HPP__