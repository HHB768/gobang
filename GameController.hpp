# ifndef  __GAMECONTROLLER_HPP__
# define  __GAMECONTROLLER_HPP__

#include "utils.hpp"
#include "Player.hpp"
#include "ChessBoard.hpp"
#include "HumanPlayer.hpp"
#include "RobotPlayer.hpp"

namespace mfwu {

template <typename Player1, typename Player2, BoardSize Size,
          typename std::enable_if<std::is_base_of<mfwu::Player, Player1>::value 
                                  && std::is_base_of<mfwu::Player, Player2>::value, int
                                 >::type = 0>
class GameController {
public:
    GameController() {
        
    }
    ~GameController();
    
    void start();  // fork a process to do this
    void game_play_task();  // for thread
    void advance();
    bool check();
private:
    ChessBoard<Size> board_;
    Player1 player1_;
    Player2 player2_;
};
}  // endof namespace mfwu

#endif  // __GAMECONTROLLER_HPP__