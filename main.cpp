#include "GameController.hpp"
using namespace mfwu;

#define __CMD_MODE__

int main() {
#ifdef __CMD_MODE__
    const GameMode  mode = print_mode_choice_help_cmd();
    const BoardSize size = print_mode_choice_help_cmd();
#else  // GUI_HELP
    const size_t mode = print_mode_choice_help_gui();
    const BoardSize size = print_mode_choice_help_gui();
#endif  // __CMD_MODE__

    GameController_base_base* game = nullptr;

    switch (std::make_pair<GameMode, BoardSize>(mode, size)) {
    case std::pair<GameMode, BoardSize>(GameMode::PVE, BoardSize::Small) : {
#ifdef __CMD_MODE__
        game = new GameController<HumanPlayer, DebugRobot, CMDBoard<BoardSize::Small>>();
#else  // __GUI_MODE__
        game = new GameController<HumanPlayer, DebugRobot, GUIBoard<BoardSize::Small>>();
#endif  // __CMD_MODE__
    } break;
    case std::pair<GameMode, BoardSize>(GameMode::PVE, BoardSize::Small) : {
#ifdef __CMD_MODE__
                game = new GameController<HumanPlayer, DebugRobot, CMDBoard<BoardSize::Small>>();
#else  // __GUI_MODE__
                game = new GameController<HumanPlayer, DebugRobot, GUIBoard<BoardSize::Small>>();
#endif  // __CMD_MODE__ 
    } break;
    }

    
    
    

    return 0;
}