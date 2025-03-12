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

        return {};
    }
    float calc_pos(int row, int col, int color) const {

        return 0;
    }
};  // endof class HumanLikeRobot

class SmartRobot : public RobotPlayer {

};  // enof class SmartRobot


}  // endof namespace mfwu

#endif // __ROBOTPLAYER_HPP__