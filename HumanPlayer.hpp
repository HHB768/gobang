#ifndef __HUMANPLAYER_HPP_
#define __HUMANPLAYER_HPP_

#include "Player.hpp"

namespace mfwu {

class HumanPlayer : public Player {
public:
    virtual CommandType play() override {
        Command cmd = this->board_->get_command();
        switch (cmd.type) {
        case CommandType::PIECE : {
            this->place(cmd.pos);
        } break;
        case CommandType::RESTART : {
        } break;
        case CommandType::MENU : {
        } break;
        case CommandType::QUIT : {
        } break;
        default :
            std::cerr << UNKNOWN_COMMAND_TYPE << "\n";
        }
        return cmd.type;
    }

    void place(const Position& pos) override {
        return Player::place(pos);
    }
protected:
    // virtual Command get_command() = 0;
    // 不应该由你来实现吧，不然真要写GUIplayer和CMDplayer你又不乐意
    // XQ4 25.03.01
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