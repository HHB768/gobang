# ifndef  __GAMECONTROLLER_HPP__
# define  __GAMECONTROLLER_HPP__

#include "utils.hpp"
#include "Player.hpp"
#include "ChessBoard.hpp"
#include "HumanPlayer.hpp"
#include "RobotPlayer.hpp"
#include <sys/wait.h>

namespace mfwu {

class GameController_base_base {
public:
    virtual GameStatus start() = 0;
    virtual void reset_game() = 0;
};  // endof class GameController_base_base

template <typename Player1_type, typename Player2_type, typename ChessBoard_type>  // TODO: type check
class GameController_base : public GameController_base_base {
public:
    GameController_base() 
        : board_(new ChessBoard_type()), 
          player1_(&board_, Piece::Color::White), 
          player2_(&board_, Piece::Color::Black),
          current_player_(player1_),
          current_player_(player2_),
          player1_first_(true),
          logger_(), archive_() {
        init_game();
    }
    ~GameController_base() {}
    
    virtual GameStatus start() {
        CommandType cmd_type;
        std::thread t(GameController_base::game_play_task, this, cmd_type);  // we really need this?
        t.join();
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
            std::cerr << "game end with status: " << status << "\n"; 
        }
    }
    
    virtual void game_play_task(CommandType& cmd_type) {
        while (!check_end()) {
            cmd_type = advance();
            if (cmd_type != CommandType::PIECE) {
                return ;
            }
        }
    }
    
    virtual CommandType advance() {
        CommandType cmd_type = current_player_->play();
        if (cmd_type == CommandType::PIECE) {
            board_->refresh();
            std::swap(current_player_, idle_player_);
        }
        return cmd_type;
    }

    virtual bool check_end() const {
        const Piece& p = board_->get_last_piece();
        ChessBoard_type::count_res_4 res;
        board_->count_dir(p, &res);
        if (is_end(res)) { return true; }
        else return false;
    }
    virtual void init_game() {
        board_->show();
        player1_first_ = true;
        current_player_ = player1_;
        idle_player_ = player2_;
        logger_.new_game();
        archive_.flush();
    }
    virtual void reset_game() {
        board_->clear();
        std::swap(player1_.get_color(), player2_.get_color());
        player1_first_ = !player1_first_;
        if (player1_first_) {
            current_player_ = player1_;
            idle_player_ = player2_;
        } else {
            current_player_ = player2_;
            idle_player_ = player1_;
        }
        logger_.new_game();
        archive_.flush();
    }
private:
    static bool is_end(const count_res_4& res) {
        if (res.left_right >= 5
            || res.up_down >= 5
            || res.up_left_down_right >= 5
            || res.up_right_down_left >= 5) {
            return true;
        } else return false;
    }

    std::shared_ptr<ChessBoard_base> board_;
    Player1_type player1_;
    Player2_type player2_;
    Player& current_player_;
    Player& idle_player_;
    bool player1_first_;

    Logger logger_;
    Archive archive_;
    // std::vector<typename ChessBoard_type::Archive_type> archive_; 

};  // endof class GameController_base

template <typename Player1_type, typename Player2_type, typename ChessBoard_type>
class GameController : public GameController_base<Player1_type, Player2_type, ChessBoard_type>;

template <typename Player1_type, typename Player2_type, BoardSize Size>
class GameController<Player1_type, Player2_type, GUIBoard<Size>> : public GameController_base<Player1_type, Player2_type, ChessBoard_type> {

};


template <typename Player1_type, typename Player2_type, BoardSize Size>
class GameController<Player1_type, Player2_type, CMDBoard<Size>> : public GameController_base<Player1_type, Player2_type, ChessBoard_type> {
public:

    
private:
    
};
}  // endof namespace mfwu

#endif  // __GAMECONTROLLER_HPP__