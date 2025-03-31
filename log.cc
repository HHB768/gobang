#include <iostream>
#include <filesystem>
#include <chrono>
#include <fstream>
#include "common.hpp"
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
            ofs_ << buf << "\n";
        }
    }
private:
    std::ifstream ifs_;
    std::ofstream ofs_;
    std::string in_filename_, out_filename_;
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
        std::cerr << "usage ./log [input_file_path] [output_file_path]\n"; 
    }
    mfwu::LogExplainer loge(in_filename, out_filename);
    loge.transform();
    return 0;
}