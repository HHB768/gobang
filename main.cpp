#include "GameController.hpp"
using namespace mfwu;

#define __CMD_MODE__

int main() {
#ifdef __CMD_MODE__
    GameMode  mode = print_mode_choice_help_cmd();
    BoardSize size = print_size_choice_help_cmd();
#else  // GUI_HELP
    GameMode mode = print_mode_choice_help_gui();
    BoardSize size = print_size_choice_help_gui();
#endif  // __CMD_MODE__

    GameController_base_base* game = nullptr;
    // use std::unique_ptr, but nonsense

#ifdef __CMD_MODE__
#define __CHESSBOARD_TYPE__ CMDBoard
    using Human_type = CMDPlayer;
#else 
#define __CHESSBOARD_TYPE__ GUIBoard
    using Human_type = GUIPlayer;
#endif

    using Robot_type = DebugRobot;
    

    switch (mode) {
    case GameMode::PVE : {
        switch (size) {
        case BoardSize::Small : {
            game = new GameController<Human_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Small>>();
        }; break;
        case BoardSize::Middle : {
            game = new GameController<Human_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Middle>>();
        }; break;
        case BoardSize::Large : {
            game = new GameController<Human_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Large>>();
        }; break;
        default:
            std::cerr << "new GC error\n";
        }; break;
    }; break;
    case GameMode::PVP : {
        switch (size) {
        case BoardSize::Small : {
            game = new GameController<Human_type, Human_type, __CHESSBOARD_TYPE__<BoardSize::Small>>();
        }; break;
        case BoardSize::Middle : {
            game = new GameController<Human_type, Human_type, __CHESSBOARD_TYPE__<BoardSize::Middle>>();
        }; break;
        case BoardSize::Large : {
            game = new GameController<Human_type, Human_type, __CHESSBOARD_TYPE__<BoardSize::Large>>();
        }; break;
        default:
            std::cerr << "new GC error\n";
        }; break;
    }; break;
    case GameMode::EVE : {
        switch (size) {
        case BoardSize::Small : {
            game = new GameController<Robot_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Small>>();
        }; break;
        case BoardSize::Middle : {
            game = new GameController<Robot_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Middle>>();
        }; break;
        case BoardSize::Large : {
            game = new GameController<Robot_type, Robot_type, __CHESSBOARD_TYPE__<BoardSize::Large>>();
        }; break;
        default:
            std::cerr << "new GC error\n";
        }; break;
    }; break;
    default:
        std::cerr << "new GC error\n";
    }
    
    while (true) {
        game->start();
        game->reset_game();
    }

    return 0;
}