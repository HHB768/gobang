#ifndef __ROBOTPLAYER_HPP__
#define __ROBOTPLAYER_HPP__

#include "Player.hpp"

namespace mfwu {

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
        
        bool is_clear_flag = true;
        for (int i = 0; i < sz; i++) {
            for (int j = 0; j < sz; j++) {
                if (this->board_->get_status(i, j)) {
                    is_clear_flag = false;
                }
            }
        }
        if (is_clear_flag) return {(int)sz / 2, (int)sz / 2};

        auto deduction_board = this->board_->snap();
        DisplayFrameworkLight board_log(deduction_board);
        auto [_, best_row, best_col] = get_best(INFERENCE_DEPTH, this->player_color_, deduction_board, board_log);
        return {best_row, best_col};
    }
    struct cmp {
        bool operator()(const std::tuple<float, int, int>& a, const std::tuple<float, int, int>& b) const {
            return std::get<0>(a) > std::get<0>(b);
        }
    };  // endof struct cmp
    std::tuple<float, int, int> get_best(int depth, Piece::Color color, 
                                         std::vector<std::vector<size_t>>& deduction_board,
                                         DisplayFrameworkLight& board_log) const {
        // if (depth == 0) return get_best(color);
        std::priority_queue<std::tuple<float, int, int>, std::vector<std::tuple<float, int, int>>, cmp> pq;
        int num_of_choices = 3;  // 3
        size_t sz = deduction_board.size();
        assert(deduction_board.size() > 0 && deduction_board.size() == deduction_board[0].size());
        std::vector<std::vector<float>> score_board(sz, std::vector<float>(sz, 0.0F));  
        // 搞一个score_board把结果存下来的意义在哪呢：debug很好用:D
        
        // 先筛选出最有价值的三个点，后面再详细看
        for (int row = 0; row < sz; row++) {
            for (int col = 0; col < sz; col++) {
                if (deduction_board[row][col] != 0) { continue; }  // only search empty pos
                score_board[row][col] += calc_pos(row, col, color)
                    + 0.6 * calc_pos(row, col, Piece::Color{Piece::get_op_real_status(color)});
                if (pq.empty() || score_board[row][col] - std::get<0>(pq.top()) > 0 - eps) {
                    while (pq.size() >= num_of_choices) {
                        pq.pop();
                    }
                    pq.emplace(score_board[row][col], row, col);
                }
            }
        }
        float max_score = INT_MIN / 2;
        int best_row = -1, best_col = -1;
        int best_score_num = 1;
        while (!pq.empty()) {
            auto [now_score, row, col] = pq.top();
            pq.pop();
            log_infer(depth, "pq_top pos: [%d, %d], score: %.2f", row, col, now_score);
            if (depth <= 0) {
                log_infer(XQ4GB_TIMESTAMP, depth, "max depth met");
            } else {
                log_infer(depth, "[1] infering %s player's optional pos: [%d, %d]",
                          Piece::get_real_status(color) == 
                          static_cast<size_t>(Piece::Color::Black) ? "black" : "white", 
                          row, col);
                deduction_board[row][col] = Piece::get_real_status(color);
                board_log.update(row, col, Piece::get_real_status(color) + 1);
                board_log.show_infer(depth, deduction_board);
                auto [op_score, op_row, op_col] = get_best(depth - 1, Piece::Color{Piece::get_op_real_status(color)},  // Piece::Color{Piece::get_op_real_status(color)}
                                                           deduction_board, board_log);  // 不应该反向吗？就像这样↑
                if (op_row < 0 || op_col < 0) {
                    deduction_board[row][col] = 0;
                    board_log.update(row, col, 0);
                    continue;
                }
                log_infer(depth, "[2] infering %s player's optional pos: [%d, %d]",
                          Piece::get_op_real_status(color) == 
                          static_cast<size_t>(Piece::Color::Black) ? "black" : "white", 
                          op_row, op_col);
                deduction_board[op_row][op_col] = Piece::get_op_real_status(color);
                board_log.update(op_row, op_col, Piece::get_op_real_status(color) + 1);
                board_log.show_infer(depth, deduction_board);
                auto [_, next_row, next_col] = get_best(depth - 1, color,
                                                        deduction_board, board_log);
                if (next_row < 0 || next_col < 0) {
                    deduction_board[op_row][op_col] = 0;
                    deduction_board[row][col] = 0;
                    board_log.update(op_row, op_col, 0);
                    board_log.update(row, col, 0);
                    continue;
                }
                log_infer(depth, "[2] infering %s player's optional pos: [%d, %d]",
                          Piece::get_real_status(color) == 
                          static_cast<size_t>(Piece::Color::Black) ? "black" : "white", 
                          next_row, next_col);
                deduction_board[next_row][next_col] = Piece::get_real_status(color);
                board_log.update(next_row, next_col, Piece::get_real_status(color) + 1);
                board_log.show_infer(depth, deduction_board);
                float next_eval = calc_pos(row, col, color)
                    + 0.6 * calc_pos(row, col, Piece::Color{Piece::get_op_real_status(color)});
                // 这里不对，推演的点在deduction board上，calc_pos没用
                // 可是我已经把这个点填上了，不就是0吗：不是，因为calc_pos记录的是board上的数据
                now_score += 0.2 * next_eval;
                deduction_board[next_row][next_col] = 0;
                deduction_board[op_row][op_col] = 0;
                deduction_board[row][col] = 0;
                board_log.update(next_row, next_col, 0);
                board_log.update(op_row, op_col, 0);
                board_log.update(row, col, 0);
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
        size_t sz = this->board_->size();
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