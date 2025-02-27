#ifndef __HUMANPLAYER_HPP_
#define __HUMANPLAYER_HPP_

#include "Player.hpp"

namespace mfwu {

class HumanPlayer : public Player {
public:
    virtual void play() override {
        Position pos = this->get_pos();
        this->place(pos);
    }

    void place(const Position& pos) override {
        return Player::place(pos);
    }
protected:
    virtual Position get_pos() = 0;
};  // endof class HumanPlayer

class GUIPlayer : public HumanPlayer {
private:
    
};  // endof class GUIPlayer

class CMDPlayer : public HumanPlayer {

};  // endof class CMDPlayer

// TODO: do we really need this?
//       or we can wait_movement() and call chessboard(GUI/CMD) to monitor the human input

}  // endof namespace mfwu

#endif  // __HUMANPLAYER_HPP__