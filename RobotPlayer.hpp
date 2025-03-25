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
        int best_score_num = 0;
        float max_score = -1.0F;
        int best_row = -1, best_col = -1;
        bool is_clear_flag = true;
        for (int i = 0; i < sz; i++) {
            for (int j = 0; j < sz; j++) {
                if (this->board_->get_status(i, j)) {
                    is_clear_flag = false;
                    continue;
                }
                float score = calc_pos(i, j);
                if (std::fabs(score - max_score) <= eps) {
                    best_score_num++;
                    if (rand() % best_score_num < 1) {
                        best_row = i;
                        best_col = j;
                    }
                } else if (score > max_score) {
                    best_score_num = 1;
                    max_score = score;
                    best_row = i;
                    best_col = j;
                } else {
                    // score < max_score
                }
            }
        }
        if (is_clear_flag) return {(int)sz / 2, (int)sz / 2};
        return {best_row, best_col};
    }
    float calc_pos(int row, int col) const {
        std::vector<int> res;
        res.reserve(4);
        for (auto&& [inc_r, inc_c] : half_dirs) {
            res.emplace_back(search_dir_rank(row, col, inc_r, inc_c));
        }
        std::sort(res.begin(), res.end());

        if (res[0] == 0) return score_map[0];
        else if (res[0] == 1) return score_map[1];
        else if (res[0] == 2 && res[1] == 2) return score_map[1] * 0.1;
        else if (res[0] == 2 && res[1] == 3) return score_map[2] * 1.5;
        return 0.8 * score_map[res[0]] + 0.2 * score_map[res[1]];
    }

    int search_dir_rank(int row, int col, int inc_r, int inc_c) const {
        assert(this->board_->get_status(row, col) == 0);
        int seq = 1, emp = 0, jump1 = 0, jump2 = 0;
        
        search_one_dir(row, col, inc_r,  inc_c, seq, emp, jump1);
        search_one_dir(row, col, -inc_r, -inc_c, seq, emp, jump2);
        
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
        } else if (seq == 3 && emp == 2 && (jump1 || jump2) 
                   || seq == 1 && emp == 2 && (jump1 == 2 || jump2 == 2)) {
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
        }
        return rank;
    }
    void search_one_dir(int row, int col, int inc_r, int inc_c, 
                        int& seq, int& emp, int& jump) const {
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
                    if (this->board_->get_status(cur_r, cur_c)
                        == Piece::get_real_color(this->player_color_)) {
                        jump++;
                    } else { break; }
                }
                break;
            } else if (this->board_->get_status(cur_r, cur_c) 
                       == Piece::get_real_color(this->player_color_)) {
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