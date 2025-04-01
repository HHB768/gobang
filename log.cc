#include <iostream>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <sstream>
#include "common.hpp"
#include "CmdFramework.hpp"
// #include "Logger.hpp"

namespace mfwu {

class LogExplainer {
public:
    LogExplainer(const std::string& in_filename,
                 const std::string& out_filename)
        : in_filename_(in_filename), out_filename_(out_filename),
          ifs_(), ofs_() {
        ifs_.open(in_filename_, std::ios::in);
        ofs_.open(out_filename_, std::ios::out);
    }
    ~LogExplainer() {
        if (ifs_.is_open()) {
            ifs_.close();
        }
        if (ofs_.is_open()) {
            ofs_.close();
        }
    }

    void transform() {
        if (!ifs_.is_open()) {
            ifs_.open(in_filename_, std::ios::in);
            if (!ifs_.is_open()) { std::cerr << "ifs open fail\n"; return ;}
        }
        if (!ofs_.is_open()) {
            ofs_.open(out_filename_, std::ios::out);
            if (!ofs_.is_open()) { std::cerr << "ofs open fail\n"; return ;}
        }
        constexpr size_t bufsz = std::max(static_cast<size_t>(BoardSize::Large) * static_cast<size_t>(BoardSize::Large), 1024UL);
        char buf[bufsz] = {};
        while (ifs_.getline(buf, bufsz)) {
            std::stringstream ss;
            std::string str = buf;
            std::vector<std::pair<size_t, size_t>> subs = get_entry_sub(str);
            if (unlikely(subs.size() < 2)) {
                ofs_ << "Not a valid record\n";
                continue;
            } else {
                std::string_view time_str(str.data() + subs[0].first, subs[0].second - subs[0].first);
                time_t t = atol(time_str.data()) + XQ4GB_TIMESTAMP;
                char buffer[64];
                tm* info = localtime(&t);
                strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", info);
                ss << '[' << buffer << ']';  // time
                ss << "[INFER] ";
                
                auto [f, s] = subs[1];
                if (unlikely(str[f] == '{')) {
                    // boardsize
                    int cidx = -1;
                    for (int i = f; i < s; i++) {
                        if (str[i] == ',') {
                            cidx = i; break;
                        }
                    }
                    if (cidx == -1) { 
                        ofs_ << "Not a valid board scale\n"; 
                        continue;
                    }
                    board_height_ = atol(str.substr(f + 1, cidx - f - 1).data());
                    board_width_  = atol(str.substr(cidx + 1, s - cidx - 1).data());
                    // ignore the rest entries (if existing)
                    ss << "BoardSize : [" << board_height_ << ", " << board_width_ << "]";
                    board_ = std::make_unique<DisplayFrameworkLight>(
                        std::vector<std::vector<size_t>>(
                            board_height_, std::vector<size_t>(board_width_)
                        )
                    );
                } else {
                    if (unlikely(subs.size() < 3)) {
                        ofs_ << "Not a valid infer step\n"; 
                        continue;
                    }
                    size_t depth = atol(get_substr(str, subs[1]).data());
                    ss << "[Depth = " << depth << "]";
                    for (int i = 0; i < depth; i++) {
                        ss << "    ";
                    }
                    ss << " ::: ";
                    
                    bool b_err = false;
                    switch (str[subs[2].first]) {
                    case '-' : {
                        if (unlikely(str.size() < 6)) {
                            ofs_ << "Not a valid option desc\n";
                            b_err = true;
                        }
                        ss << "Prior pos: [" << get_substr(str, subs[3]) << ", "
                           << get_substr(str, subs[4]) << "], score: "
                           << get_substr(str, subs[5]);
                    } break;
                    case '!' : {
                        ss << "Max depth met";
                    } break;
                    case '*' : {
                        board_->unzip_tbl(std::string_view(str.data() + subs[3].first, subs[3].second - subs[3].first), true);
                        ss << "Deduction board\n :::";
                        for (auto&& line : board_->get_framework()) {
                            ss << get_log_space(depth);
                            ss << " ::: " << line << " ::: \n";
                        }
                    } break;
                    case ':' : {
                        board_->unzip_tbl(std::string_view(str.data() + subs[3].first, subs[3].second - subs[3].first), false);
                        ss << "Deduction board\n";
                        for (auto&& line : board_->get_framework()) {
                            ss << get_log_space(depth);
                            ss << line << "\n";
                        }
                    } break;
                    case '1' : {
                        ss << "[1] Infering " << (str[subs[3].first] == 'b' ? "black" : "white")
                           << "player's optional pos: [" << get_substr(str, subs[4]) << ", "
                           << get_substr(str, subs[5]) << "]";
                    } break;
                    case '2' : {
                        ss << "[2] Infering " << (str[subs[3].first] == 'b' ? "black" : "white")
                           << "player's optional pos: [" << get_substr(str, subs[4]) << ", "
                           << get_substr(str, subs[5]) << "]";
                    } break;
                    case '3' : {
                        ss << "[3] Infering " << (str[subs[3].first] == 'b' ? "black" : "white")
                           << "player's optional pos: [" << get_substr(str, subs[4]) << ", "
                           << get_substr(str, subs[5]) << "]";
                    } break;
                    default : {
                        ofs_ << "Not a valid type\n";
                    }
                    }
                    if (b_err) { continue; }

                    ss << " ::: ";
                }
            }
            ofs_ << ss.str() << "\n";
        }
        ofs_.flush();
    }
private:
    static std::vector<std::pair<size_t, size_t>> get_entry_sub(const std::string& str) {
        std::vector<std::pair<size_t, size_t>> ret;
        int last_idx = 0;
        for (int i = 0; i < str.size(); i++) {
            if (str[i] == ' ') {
                ret.emplace_back(last_idx, i);
                last_idx = i + 1;
            }
        }
        ret.emplace_back(last_idx, str.size());
        return ret;
    }
    // TODO: use string_view?
    static std::string get_substr(const std::string& str, const std::pair<size_t, size_t>& sub) {
        return str.substr(sub.first, sub.second - sub.first);
    }

    static std::string get_log_space(size_t depth) {
        //                 [2025-04-01 20:19:28][INFER] [Depth = 0] 
        std::string ret = "                                        ";
        for (int i = 0; i < depth; i++) {
            ret += "    ";
        }
        return ret;
    }

    std::ifstream ifs_;
    std::ofstream ofs_;
    std::string in_filename_, out_filename_;

    size_t board_height_;
    size_t board_width_;

    std::unique_ptr<DisplayFrameworkLight> board_ = nullptr;
};  // endof class LogExplainer

constexpr const char* dir = "./inference/";

std::string get_default_input_file_path() {
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;
    std::filesystem::directory_entry path = {};
    std::filesystem::file_time_type last_time{};
    if (!std::filesystem::exists(dir)) { return ""; }
    for (const auto& file : std::filesystem::directory_iterator(dir)) {
        if (file.is_regular_file() && file.path().extension().string() == std::string(".inf")) {
            auto this_time = std::filesystem::last_write_time(file.path());
            if (this_time > last_time 
                || last_time == std::filesystem::file_time_type{}) {
                last_time = this_time;
                path = file;
            }
        }
    }
    if (path == std::filesystem::directory_entry{}) {
        return "";
    }
    return path.path().string();
}

std::string get_default_output_file_path() {
    std::string path = dir;

    if (!std::filesystem::exists(path))
    {
        bool status = std::filesystem::create_directories(path);
        if (!status)
        {
            std::cerr << "cannot create dir\n";
            return "";
        }
    }
    path += "logE_";
    mfwu::append_time_info(path);
    path += ".infer";
    return path;
}

}  // endof namespace mfwu

int main(int argc, char** argv) {
    std::string in_filename, out_filename;
    if (argc == 1) {
        in_filename = mfwu::get_default_input_file_path();
        out_filename = mfwu::get_default_output_file_path();
    } else if (argc == 2) {
        in_filename = argv[1];
        out_filename = mfwu::get_default_output_file_path();
    } else if (argc == 3) {
        in_filename = argv[1];
        out_filename = argv[2];
    } else {
        std::cerr << "wrong arg count\n";
        std::cerr << "usage: ./log [input_file_path] [output_file_path]\n"; 
    }
    mfwu::LogExplainer loge(in_filename, out_filename);
    loge.transform();
    return 0;
}


// legacy: 250401
// not robust str parsing
// int 
// if (str[0] == '{' && str[1] == '}') {
    
// }
// if (str[0] != '{' or str[11] != '}') {
//     ofs_ << "Time stamp not found!\n";
//     continue;
// }
// bool board_size_flag = false;
// int cidx = -1, bidx = -1, didx = -1;
// int depth = -1;
// if (unlikely(str[12] == '{')) {  // boardsize
//     board_size_flag = true;
//     if (unlikely(str[18] != '}')) {
//         // CHECK
//         for (int i = 13; i < str.size(); i++) {
//             if (str[i] == ',') {
//                 cidx = i;
//             } else if (str[i] == '}') {
//                 bidx = i;
//             }
//         }
//         if (cidx == -1 or bidx == -1 or cidx >= bidx) {
//             ofs_ << "Board scale not found!\n";
//             continue;
//         }
//     }
// } else if (likely(str[12] == '[')) {  // depth
//     if (likely(str[14] != ']')) {
//         for (int i = 13; i < str.size(); i++) {
//             if (str[i] == ']') {
//                 didx = i;
//                 break;
//             }
//         }
//     }
//     if (didx == -1) {
//         ofs_ << "Depth not found\n";
//         continue;
//     }
// }
// std::stringstream ss;
// std::string time_str = str.substr(1, 10);
// time_t log_time = atoi(time_str.c_str());
// char buffer[64];
// if (log_time == XQ4GB_TIMESTAMP) {
//     strcpy(buffer, "                     \0");
//     //              [2025-03-07 20:57:00]
//     ss << buffer;
// } else {
//     tm* info = localtime(&log_time);
//     strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", info);
//     ss << '[' << buffer << ']';
// }

// if (unlikely(board_size_flag)) {
//     if (cidx == -1 or bidx == -1 or cidx >= bidx) {
//         board_height_ = atoi(str.substr(13, 2).c_str());
//         board_width_ = atoi(str.substr(16, 2).c_str());
//     } else {
//         board_height_ = atoi(str.substr(13, cidx - 13).c_str());
//         board_width_  = atoi(str.substr(cidx + 1, bidx - cidx - 1).c_str());
//     }
//     ss << "New game with boardsize : [" << board_height_ << ", " << board_width_ << "]";
// } else {  // depth
//     if (didx == -1) {
//         depth = str[13] - '0';
//         didx = 14;
//     } else {
//         depth = atoi(str.substr(13, didx - 13).c_str());
//     }
//     didx++;

//     switch (str[didx]) {
//     case ''
//     }
// }
// ofs_ << ss.str() << "\n";