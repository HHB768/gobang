#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include "utils.hpp"

namespace mfwu {
enum class LogLevel : size_t {
    DEBUG = 0,
    INFO  = 1,
    WARN  = 2,
    ERROR = 3,
    
    TOTAL  
};  // endof enum class LogLevel


struct LogMsg {
    time_t time_stamp;
    // uint32_t pid;
    // uint64_t tid;
    std::string msg;
};  // endof struct LogMsg

class LogFormatter {
public:
    static std::string format(LogLevel level, const LogMsg& msg) {
        std::stringstream ss;
        char buffer[64];
        if (msg.time_stamp == XQ4GB_TIMESTAMP) {
            strcpy(buffer, "                     \0");
            //              [2025-03-07 20:57:00]
            ss << buffer;
        } else {
            tm* info = localtime(&msg.time_stamp);
            strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", info);
            ss << '[' << buffer << ']';
        }
        ss << LogLevelDescription.at(static_cast<size_t>(level))
           << ' ' << msg.msg << "\n";
        return ss.str();
    }
private:
    static const std::vector<std::string> LogLevelDescription;
};  // endof class LogFormatter

const std::vector<std::string> LogFormatter::LogLevelDescription = {
    "[DEBUG]", "[INFO] ","[WANR] ", "[ERROR]"
};

class LogAppender {
public:
    LogAppender(LogLevel level) : level_(level), 
        formatter_(std::make_shared<LogFormatter>()) {}

    virtual void append(LogLevel level, const LogMsg& msg) = 0;
protected:
    LogLevel level_;
    std::shared_ptr<LogFormatter> formatter_; 
};  // endof class LogAppender

class StdAppender : public LogAppender {
public:
    StdAppender(LogLevel level) : LogAppender(level) {}
    void append(LogLevel level, const LogMsg& msg) {
        if (level < this->level_) return ;
        std::string res = this->formatter_->format(level, msg);
        std::cout << res << "\n";
    }
};  // endof class StdAppender

class FileAppender : public LogAppender {
public:
    static constexpr const char* dir = "./log";
    FileAppender(LogLevel level, std::string filename="") 
        : LogAppender(level), filename_(filename), fs_() {
        if (filename == std::string("")) {
            time_t now = time(0);
            tm* lt = localtime(&now);
            std::string str = dir;
            str += '/'; str += std::to_string(1900 + lt->tm_year);
            str += '_'; str += std::to_string(1 + lt->tm_mon);
            str += '_'; str += std::to_string(lt->tm_mday);
            str += '_'; str += std::to_string(lt->tm_hour);
            str += '_'; str += std::to_string(lt->tm_min);
            str += '_'; str += std::to_string(lt->tm_sec); 
            filename_ = str;
            if (!std::filesystem::exists(dir)) {
                bool succ = std::filesystem::create_directories(dir);
                if (!succ) { 
                    std::cerr << "creating dir fails, logfile may be lost\n";
                }
            }
        }
        fs_.open(filename_, std::ios::app);
    }
    ~FileAppender() {
        if (fs_.is_open()) {
            fs_.close();
        }
    }
    void append(LogLevel level, const LogMsg& msg) {
        if (level < this->level_) return ;
        std::string res = this->formatter_->format(level, msg);
        if (!fs_.is_open()) {
            fs_.open(filename_, std::ios::app);
        }
        fs_ << res << "\n";
    }
    void flush() {
        if (!fs_.is_open()) {
            fs_.open(filename_, std::ios::app);
        }
        fs_.flush();
    }

private:
    std::fstream fs_;
    std::string filename_;
};  // endof class FileAppender

class Logger {
public:
    static Logger& Instance() {
        static Logger logger;
        return logger;
    }

    template <typename... Args>
    void log(LogLevel level, const char* fmt, Args&&... args) {
        log(level, format(fmt, std::forward<Args>(args)...));
    }
    template <typename... Args>
    void log(LogLevel level, time_t time_stamp, const char* fmt, Args&&... args) {
        log(level, time_stamp, format(fmt, std::forward<Args>(args)...));
    }
    // check: if we pass a string with const char*, 
    // should it be accepted by the first one?
    void log(LogLevel level, const std::string& msg) {
        LogMsg lmsg;
        lmsg.time_stamp = time(0);
        lmsg.msg = msg;
        std_appender_.append(level, lmsg);
        file_appender_.append(level, lmsg);
    } 
    void log(LogLevel level, time_t time_stamp, const std::string& msg) {
        LogMsg lmsg;
        lmsg.time_stamp = time_stamp;
        lmsg.msg = msg;
        std_appender_.append(level, lmsg);
        file_appender_.append(level, lmsg);
    }
    template <typename... Args>
    void log_debug(const char* fmt, Args&&... args) {
        log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void log_debug(time_t time_stamp, const char* fmt, Args&&... args) {
        log(LogLevel::DEBUG, time_stamp, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void log_info(const char* fmt, Args&&... args) {
        log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void log_info(time_t time_stamp, const char* fmt, Args&&... args) {
        log(LogLevel::INFO, time_stamp, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void log_warn(const char* fmt, Args&&... args) {
        log(LogLevel::WARN, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void log_warn(time_t time_stamp, const char* fmt, Args&&... args) {
        log(LogLevel::WARN, time_stamp, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void log_error(const char* fmt, Args&&... args) {
        log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void log_error(time_t time_stamp, const char* fmt, Args&&... args) {
        log(LogLevel::ERROR, time_stamp, fmt, std::forward<Args>(args)...);
    }
    
    void new_game(GameStatus status) {
        log(LogLevel::INFO, "game ends with status: %s", 
            GameStatusDescription.at(static_cast<size_t>(status)).c_str());
        file_appender_.flush();
    }
    void end_game(GameStatus status) {
        log(LogLevel::INFO, "game ends with status: %s", 
            GameStatusDescription.at(static_cast<size_t>(status)).c_str());
        file_appender_.flush();
    }

private:
#ifdef __CMD_MODE__
    Logger() : std_appender_(LogLevel::TOTAL), file_appender_(LogLevel::DEBUG) {}
#else  // __GUI_MODE__
    Logger() : std_appender_(LogLevel::ERROR), file_appender_(LogLevel::DEBUG) {}
#endif  // __CMD_MODE__

    ~Logger() {}
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::string format(const char* fmt, ...) const {
        int len;
        std::string str;
        va_list args;
        char buffer[256];

        va_start(args, fmt);
        if ((len = vsnprintf(buffer, sizeof(buffer), fmt, args)) > 0) {
            if (len < sizeof(buffer)) {
                str = buffer;
            } else {
                int maxsz = len + 1;
                char* buffer = (char*)malloc(maxsz);
                if (buffer) {
                    len = vsnprintf(buffer, maxsz, fmt, args);
                    if (len > 0 && len < maxsz) {
                        str = buffer;
                    }
                    free(buffer);
                }
            }
        }
        va_end(args);
        return str;
    }

    StdAppender std_appender_;
    FileAppender file_appender_;
};  // endof class Logger

template <typename... Args>
void log(LogLevel level, const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log(level, fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void log(LogLevel level, time_t time_stamp, const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log(level, time_stamp, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_debug(const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log_debug(fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void log_debug(time_t time_stamp, const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log_debug(time_stamp, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_info(const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log_info(fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void log_info(time_t time_stamp, const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log_info(time_stamp, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_warn(const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log_warn(fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void log_warn(time_t time_stamp, const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log_warn(time_stamp, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_error(const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log_error(fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void log_error(time_t time_stamp, const char* fmt, Args&&... args) {
    Logger& logger = Logger::Instance();
    logger.log_error(time_stamp, fmt, std::forward<Args>(args)...);
}

void log_new_game(GameStatus status) {
    Logger& logger = Logger::Instance();
    logger.new_game(status);
}
void log_end_game(GameStatus status) {
    Logger& logger = Logger::Instance();
    logger.end_game(status);
}

}  // endof namespace mfwu

#endif  // __LOGGER_HPP__