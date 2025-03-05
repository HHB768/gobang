#ifndef __ROBOTPLAYER_HPP__
#define __ROBOTPLAYER_HPP__

#include "Player.hpp"

namespace mfwu {

class RobotPlayer : public Player {
public:
    RobotPlayer() : Player() {}
    RobotPlayer(std::shared_ptr<ChessBoard_base> board, Piece::Color color) : Player(board, color) {}

    virtual CommandType play() override {
        Position pos = get_best_position();
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

};  // endof class DummyRobot

class HumanLikeRobot : public RobotPlayer {

};  // endof class HumanLikeRobot

class SmartRobot : public RobotPlayer {

};  // enof class SmartRobot


}  // endof namespace mfwu

#endif // __ROBOTPLAYER_HPP__