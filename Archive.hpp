#ifndef __ARCHIVE_HPP__
#define __ARCHIVE_HPP__

namespace mfwu {

/*
    only for game archive now
    we can develop it further to
    restore a previous game
    X 25.03.20
*/
template <typename Seq_type>
class Archive {
public:
    static constexpr const char* dir = "./archive";
    Archive(const std::string& archive_filename="") {
        if (archive_filename == std::string("")) {
            time_t now = time(0);
            tm* lt = localtime(&now);
            std::string str = dir;
            str += '/'; str += std::to_string(1900 + lt->tm_year);
            str += '_'; str += std::to_string(1 + lt->tm_mon);
            str += '_'; str += std::to_string(lt->tm_mday);
            str += '_'; str += std::to_string(lt->tm_hour);
            str += '_'; str += std::to_string(lt->tm_min);
            str += '_'; str += std::to_string(lt->tm_sec); 
            archive_filename_ = str;
            if (!std::filesystem::exists(dir)) {
                status_ = std::filesystem::create_directories(dir);
                if (!status_) { 
                    log_error("archive error: ");
                    log_error(XQ4GB_TIMESTAMP, "cannot create dir");
                }
            }
        }
        // if dir doesnt exist, fs_ wont create and open the file
        fs_.open(archive_filename_, std::ios::app);
    }
    ~Archive() {
        if (fs_.is_open()) {
            fs_.close();
        }
    }

    void record(const Seq_type& seq) {
        frames_.push_back(seq);
    }
    void record(Seq_type&& seq) {
        frames_.push_back(std::move(seq));
    }
    void flush(GameStatus status) {
        for (Seq_type& frame : frames_) {
            flush_frame(std::move(frame));  // check: can i?
        }
        frames_.clear();
        flush_log(status);
        fs_.flush();  // flush once after a game
    }
    std::string get_last_frame() const {
        return frames_.back();
    }
    void pop_last_record() const {
        frames_.pop_back();
    }
    bool get_status() const {
        return status_;
    }
private:
    void flush_log(GameStatus status) {
        if (!fs_.is_open()) {
            fs_.open(archive_filename_, std::ios::app);
        }
        fs_ << "[XQ4GB-SEP]\n"
            << "this game end with status:" 
            << GameStatusDescription.at(static_cast<size_t>(status))
            << "\n\n";
    }

    void flush_frame(Seq_type&& frame) {
        if (!fs_.is_open()) {
            fs_.open(archive_filename_, std::ios::app);
        }
        fs_ << std::move(frame) << "\n";
    }

    std::string archive_filename_;
    std::fstream fs_;
    bool status_ = true;
    
    std::vector<Seq_type> frames_;
};  // endof class Archive

}  // endof namespace mfwu

#endif  // __ARCHIVE_HPP__