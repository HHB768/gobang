#ifndef __ROBOTPLAYER_HPP__
#define __ROBOTPLAYER_HPP__

#include "Player.hpp"

namespace mfwu {

class RobotPlayer : public Player {
public:
    virtual void play() override {
        Position pos = get_best_position();
        this->place(pos);
    }

    void place(const Position& pos) override {
        Player::place(pos);
    }

protected:
    virtual Position get_best_position() const = 0;
};  // endof class RobotPlayer

class DebugRobot : public RobotPlayer {
private:
    Position get_best_position() const override {
        size_t len = this->board_->size();
        int row = -1, col = -1;
        while (is_valid(row, col) == false) {
            row = rand() % len;
            col = rand() % len;
        }
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