# ifndef  __GAMECONTROLLER_HPP__
# define  __GAMECONTROLLER_HPP__

#include "utils.hpp"
#include "Player.hpp"
#include "ChessBoard.hpp"
#include "HumanPlayer.hpp"
#include "RobotPlayer.hpp"
#include "Logger.hpp"
#include "Archive.hpp"
#include <sys/wait.h>

namespace mfwu {

class GameController_base_base {
public:
    virtual GameStatus start() = 0;
    virtual void restart_game_init() = 0;
    virtual void reset_game_init() = 0;
};  // endof class GameController_base_base

template <typename Player1_type, typename Player2_type, typename ChessBoard_type>  // TODO: type check
class GameController_base : public GameController_base_base {
public:
    GameController_base() 
        : board_(std::make_shared<ChessBoard_type>()), 
          player1_(board_, Piece::Color::White), 
          player2_(board_, Piece::Color::Black),
          current_player_(&player1_),
          idle_player_(&player2_),
          player1_first_(true),
          logger_(), archive_() {
        init_game();
    }
    virtual ~GameController_base() {}
    
    virtual GameStatus start() {
        this->board_->show();
        CommandType cmd_type;
        // std::thread t(&GameController_base::game_play_task, this, std::ref(cmd_type));  // we really need this?
        this->game_play_task(cmd_type);
        // t.join();
        switch (cmd_type) {
        case CommandType::PIECE : {
            return GameStatus::NORMAL;
        } break;
        case CommandType::RESTART : {
            return GameStatus::RESTART;
        } break;
        case CommandType::MENU : {
            return GameStatus::MENU;
        } break;
        case CommandType::QUIT: {
            return GameStatus::QUIT;
        } break;
        default:
            std::cerr << "game end with cmd type: " << static_cast<size_t>(cmd_type) << "\n"; 
        }
        return GameStatus::INVALID;
    }
    
    virtual void game_play_task(CommandType& cmd_type) {
        do {
            cmd_type = this->advance();
            if (cmd_type != CommandType::PIECE) {
                if (cmd_type == CommandType::XQ4GB) {
                    // logger_.new_game();
                    archive_.flush();
                    execl("./xq4gb", "xq4gb", NULL);
                    exit(0x3F3F3F3F);
                }
                return ;
            }
        } while (!this->check_end());
        // only victory comes here
        winner_display(idle_player_->get_color());
    }
    
    virtual CommandType advance() {
        CommandType cmd_type = current_player_->play();
        if (cmd_type == CommandType::PIECE) {
            board_->refresh();
            Player* temp = current_player_;
            current_player_ = idle_player_;
            idle_player_ = temp;
        }
        return cmd_type;
    }

    virtual bool check_end() const {
        const Piece& p = board_->get_last_piece();
        count_res_4 res;
        board_->count_dir(p, &res);
        if (is_end(res)) { return true; }
        else return false;
    }
    virtual void init_game() {
        player1_first_ = true;
        current_player_ = &player1_;
        idle_player_ = &player2_;
        logger_.new_game();
        archive_.flush();
        board_->show();
    }
    virtual void restart_game_init() {
        board_->reset();
        // doesnt swap
        if (player1_first_) {
            current_player_ = &player1_;
            idle_player_ = &player2_;
        } else {
            current_player_ = &player2_;
            idle_player_ = &player1_;
        }
        logger_.new_game();
        archive_.flush();
        // board_->show();
    }
    virtual void reset_game_init() {
        board_->reset();
        std::swap(player1_.get_color(), player2_.get_color());
        player1_first_ = !player1_first_;
        if (player1_first_) {
            current_player_ = &player1_;
            idle_player_ = &player2_;
        } else {
            current_player_ = &player2_;
            idle_player_ = &player1_;
        }
        logger_.new_game();
        archive_.flush();
        // board_->show();
    }
protected:
    virtual void winner_display(const Piece::Color& color) const {
        board_->winner_display(color);
    }
private:
    static bool is_end(const count_res_4& res) {
        if (res.left_right >= NEED - 1
            || res.up_down >= NEED - 1
            || res.up_left_down_right >= NEED - 1
            || res.up_right_down_left >= NEED - 1) {
            return true;
        } else return false;
    }
    
    std::shared_ptr<ChessBoard_base> board_;
    Player1_type player1_;
    Player2_type player2_;
    Player* current_player_;  // doesnt alloc any mem
    Player* idle_player_;
    bool player1_first_;

    Logger logger_;
    Archive<std::string> archive_;
    // std::vector<typename ChessBoard_type::Archive_type> archive_; 

};  // endof class GameController_base

template <typename Player1_type, typename Player2_type, typename ChessBoard_type>
class GameController : public GameController_base<Player1_type, Player2_type, ChessBoard_type> {
    GameController() : GameController_base<Player1_type, Player2_type, ChessBoard_type>() {}
    virtual ~GameController() {}
};

// template <typename Player1_type, typename Player2_type, BoardSize Size>
// class GameController<Player1_type, Player2_type, GUIBoard<Size>> : public GameController_base<Player1_type, Player2_type, GUIBoard<Size>> {
// public:
//     GameController<Player1_type, Player2_type, GUIBoard<Size>>()
//         : GameController_base<Player1_type, Player2_type, GUIBoard<Size>>() {}
//     virtual ~GameController<Player1_type, Player2_type, GUIBoard<Size>>() {}
// };


template <typename Player1_type, typename Player2_type, BoardSize Size>
class GameController<Player1_type, Player2_type, CMDBoard<Size>> : public GameController_base<Player1_type, Player2_type, CMDBoard<Size>> {
public:
    GameController<Player1_type, Player2_type, CMDBoard<Size>>() 
        : GameController_base<Player1_type, Player2_type, CMDBoard<Size>>() {}
    virtual ~GameController<Player1_type, Player2_type, CMDBoard<Size>>() {}
    
private:
    
};
}  // endof namespace mfwu

#endif  // __GAMECONTROLLER_HPP__