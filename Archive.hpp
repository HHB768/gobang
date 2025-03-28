#ifndef __ARCHIVE_HPP__
#define __ARCHIVE_HPP__

#include "common.hpp"
#include "Logger.hpp"

namespace mfwu {

/*
    only for game archive now
    we can develop it further to
    restore a previous game
    X 25.03.20
*/
/*
    why i dont ref to chessboard directly
    bcz i want a independent status-only board here
    simple enough, stable enough
    XQ 25.03.27
*/
template <BoardSize Size = BoardSize::Small,
          typename Seq_type=std::string, 
          typename Tbl_type=std::vector<std::vector<int>>
         >
class Archive_base {
public:
    static constexpr const char* dir = "./archive";
    Archive_base(const std::string& archive_filename="") {
        if (archive_filename == std::string("")) {
            std::string str = dir;
            str += '/'; 
            append_time_info(str);
            str += ".arc";
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
    ~Archive_base() {
        if (fs_.is_open()) {
            fs_.close();
        }
    }
    virtual void init_game() = 0;
    virtual void init_game(const Tbl_type& board) = 0;
    virtual void record(const Str_type& seq) = 0;
    virtual void record(Str_type&& seq) = 0;
    virtual void record(const Tbl_type& tbl) = 0;
    virtual void record(Tbl_type&& tbl) = 0;
    virtual void record(const Piece last_piece) = 0;

    virtual void flush(GameStatus status) = 0;
    virtual std::string get_last_frame_in_seq() = 0;
    virtual void pop_last_n_record(int num=1) = 0;

    bool get_status() const {
        return status_;
    }
protected:
    void flush_log(GameStatus status) {
        if (!fs_.is_open()) {
            fs_.open(archive_filename_, std::ios::app);
        }
        fs_ << "[XQ4GB-SEP]\n"
            << "this game end with status:" 
            << GameStatusDescription.at(static_cast<size_t>(status))
            << "\n\n";
    }
    void flush_frame(Frame& frame) {
        if (!fs_.is_open()) {
            fs_.open(archive_filename_, std::ios::app);
        }
        fs_ << std::move(frame.get_seq()) << "\n";
    }
    static void remove_sp(Tbl_type& tbl) {
        bool found_sp_flag = false;
        for (auto& line : tbl) {
            for (size_t& status : line) {
                if (status == static_case<size_t>(Piece::Color::BlackSp)
                    || status == static_cast<size_t>(Piece::Color::WhiteSp)) {
                    status--;
                    if (found_sp_flag) {
                        log_warn("Multiple sp pieces found in remove_sp()");
                    } else {
                        found_sp_flag = true;
                    }
                }
            }
        }
        if (!found_sp_flag) { log_warn("No sp piece found in remove_sp()"); }
    }
    static void update_board_with_piece(Tbl_type& tbl, const Piece& p) {
        remove_sp(frame.tbl);
        frame.tbl[last_piece.row][last_piece.col] = last_piece.get_real_color() + 1;
    }

    struct Frame {
        bool is_seq_valid = false;
        bool is_tbl_valid = false;
        Seq_type seq = {};
        Tbl_type tbl = {};

// #define __RESERVE_MEM_WEAK__    
// #define __RESERVE_MEM_STRONG__
// #ifdef __RESERVE_MEM_STRONG__
// #define __RESERVE_MEM_WEAK__  
// #endif // __RESERVE_MEM_STRONG__
//         Frame() : is_seq_valid(false), is_tbl_valid(false),
//                   seq(),
// #ifdef __RESERVE_MEM_STRONG__
//                   tbl(Size_hint)
// #else  // !__RESERVE_MEM_STRONG__
//                   tbl()
// #endif  // __RESERVE_MEM_STRONG__
//         {
// #ifdef __RESERVE_MEM_WEAK__
//             seq.reserve(Size_hint * (Size_hint + 1) * 2 + 64);  // check
//             for (auto& line : tbl) {
//                 line.reserve(Size_hint);
//             }
// #endif // __RESERVE_MEM_WEAK__
//         }
// #undef __RESERVE_MEM_WEAK__
// #undef __RESERVE_MEM_STRONG__
        
        Frame() = default;
        Frame(const Seq_type& seq_)
            : is_seq_valid(true), is_tbl_valid(false),
              seq(seq_), tbl() {}
        Frame(Seq_type&& seq_)
            : is_seq_valid(true), is_tbl_valid(false),
              seq(std::move(seq_)), tbl() {}
        Frame(const Tbl_type& tbl)
            : is_seq_valid(false), is_tbl_valid(true),
              seq(), tbl(tbl_) {}
        Frame(Tbl_type&& tbl)
            : is_seq_valid(false), is_tbl_valid(true),
              seq(), tbl(std::move(tbl_)) {}

        void update(const Seq_type& seq_) {
            seq = seq_;
            is_seq_valid = true;
        }
        void update(Seq_type&& seq_) {
            seq = std::move(seq_);
            is_seq_valid = true;
        }
        void update(const Tbl_type& tbl_) {
            tbl = tbl_;
            is_tbl_valid = true;
        }
        void update(Tbl_type&& tbl_) {
            tbl = std::move(tbl_);
            is_tbl_valid = true;
        }

        void serialize() {
            if (is_seq_valid) return ;
            assert(is_tbl_valid);
            seq.clear();
            seq.reserve(Size_hint * (Size_hint + 1) * 2);  // check
            for (size_t i = 0; i < tbl.size(); i++) {
                for (size_t j = 0; j < tbl[0].size(); j++) {
                    seq += this->board_[i][j]->get_status();
                    seq += ' ';
                }
                seq += '\n';
            }
            is_seq_valid = true;
        }
        void deserialize() {
            if (is_tbl_valid) return ;
            assert(is_seq_valid);
            tbl.clear();
            int i = 0;
            tbl.reserve(Size_hint);  // check
            for (int k = 0; k < seq.size(); k++) {
                if (is_digit(seq[k])) {
                    if (i >= tbl.size()) {
                        tbl.resize(i + 1);  // check
                        tbl[i].reserve(Size_hint);  // check
                    }
                    tbl.push_back(seq[k]);
                } else if (seq[k] == '\n') {
                    i++;
                } else {
                    assert(seq[k] == ' ');
                }
            }
            is_tbl_valid = true;
        }
    
        Seq_type& get_seq() {
            serialize();
            return seq;
        }
        Tbl_type& get_tbl() {
            deserialize();
            return tbl;
        }
    };  // endof struct Frame

    std::vector<Frame> frames_;
private:
    std::string archive_filename_;
    std::fstream fs_;
    bool status_ = true;
};  // endof class Archive_base

// always sync frames_ in record()
template <BoardSize Size = BoardSize::Small,
          typename Seq_type=std::string, 
          typename Tbl_type=std::vector<std::vector<int>>
         >
class Archive {
public:
    Archive(const std::string& archive_filename="") 
        : Archive_base<Size, Seq_type, Tbl_type>(archive_filename) {}
    ~Archive() {}

    void record(const Str_type& seq) override {
        this->frames_.emplace_back(seq);
    }
    void record(Str_type&& seq) override {
        this->frames_.emplace_back(std::move(seq));
    }
    void record(const Tbl_type& tbl) override {
        this->frames_.emplace_back(tbl);
    }
    void record(Tbl_type&& tbl) override {
        this->frames_.emplace_back(std::move(tbl));
    }
    void record(const Piece& last_piece) override {
        Frame frame = get_last_frame_in_tbl();
        // if the pattern of serialization is constant
        // theoretically we can locate the pos in seq
        // let's see...  X-Q4 25.03.27
        this->update_board_with_piece(frame.tbl, last_piece);
    }
    // warning: will destroy all the frames!
    void flush(GameStatus status) override {
        for (Frame& frame : frames_) {
            flush_frame(frame);
        }
        flush_log(status);
        fs_.flush();  // flush once after a game

        // reinit for next game
        this->init_game();
    }
    Seq_type& get_last_frame_in_seq() override {
        return frames_.back().get_seq();
    }
    Tbl_type& get_last_frame_in_tbl() override {
        return frames_.back().get_tbl();
    }
    void pop_last_n_record(int num=1) override {
        for (int i = 0; i < num; i++) {
            frames_.pop_back();     
        }
    }

    void init_game() override {
        frames_.clear();
        frames_.emplace(Tbl_type(Size, Tbl_type::value_type(Size, 0)));
    }
    void init_game(const Tbl_type& board) override {
        frames_.clear();
        frames_.emplace(board);
    }
};  // endof class Archive

// update only moves_ in record()
// update frames_ when needed
// less record cost
// more deduction cost
template <BoardSize Size = BoardSize::Small,
          typename Seq_type=std::string, 
          typename Tbl_type=std::vector<std::vector<int>>
         >
class ArchiveLight {
public:
    ArchiveLight(const std::string& archive_filename="") 
        : Archive_base<Size, Seq_type, Tbl_type>(archive_filename) {}
    ~ArchiveLight() {}

    void record(const Str_type& seq) override {  // can i delete it?  0327
        update_frames();
        this->frames_.emplace_back(seq);
        update_last_move();
        assert(this->frames_.size() == this->moves_.size());
    }
    void record(Str_type&& seq) override {
        update_frames();
        this->frames_.emplace_back(std::move(seq));
        update_last_move();
        assert(this->frames_.size() == this->moves_.size());
    }
    void record(const Tbl_type& tbl) override {
        update_frames();
        this->frames_.emplace_back(tbl);
        update_last_move();
        assert(this->frames_.size() == this->moves_.size());
    }
    void record(Tbl_type&& tbl) override {
        update_frames();
        this->frames_.emplace_back(std::move(tbl));
        update_last_move();
        assert(this->frames_.size() == this->moves_.size());
    }
    // recommended
    void record(const Piece last_piece) {
        moves_.emplace_back(last_piece);
    }

    void flush(GameStatus status) {
        for (Frame& frame : frames_) {
            flush_frame(frame);
        }
        frames_.clear();
        flush_log(status);
        fs_.flush();  // flush once after a game

        // reinit for next game
        this->init_game();
    }
    std::string get_last_frame_in_seq() const {
        update_frame();
        return frames_.back().get_seq();
    }
    void pop_last_n_record(int num=1) {
        for (int i = 0; i < num; i++) {
            moves_.pop_back();    
        }
        while (this->frames_.size() > moves_.size()) {
            frames_.pop_back();
        }
        update_frame();
    }

    void init_game() override {
        frames_.clear();
        frames_.emplace(Tbl_type(Size, Tbl_type::value_type(Size, 0)));
        moves_.clear();
        moves_.emplace(invalid_piece);
    }
    void init_game(const Tbl_type& board) override {
        frames_.clear();
        frames_.emplace(board);
        moves_.clear();
        bool found_sp_flag = false;
        Piece p = invalid_piece;
        for (int i = 0; i < Size; i++) {
            for (int j = 0; j < Size; j++) {
                size_t status = board[i][j];
                if (status == static_case<size_t>(Piece::Color::BlackSp)
                    || status == static_cast<size_t>(Piece::Color::WhiteSp)) {
                    if (found_sp_flag) {
                        // redundant sp should be remove
                        frames_.back().get_tbl()[i][j]--;
                        log_warn("Multiple sp pieces found in init_game()");
                    } else {
                        p = Piece{i, j, Piece::Color{status}};
                        found_sp_flag = true;
                    }
                }
            }
        }
        if (!found_sp_flag) { log_warn("No sp piece found in init_game()"); }
        moves_.emplace(p);
    }
private:
    void update_frame() {
        if (this->moves_.size() == this->frame_.size()) {
            return ;
        } else if (this->moves_.size() < this->frame_.size()) {
            log_error("Archive is out of synchronism");
            log_error(XQ4GB_TIMESTAMP, "moves_.size(): %d, frames_.size(): %d", 
                      this->moves_.size(), this->frame_.size());
            // in this case, you should update_last_move first
        } else {  // this->moves_.size() > this->frame_.size()
            Tbl_type board = this->frames_.back().get_tbl();
            assert(this->frames_.size());
            for (int i = this->frames_.size(); i < this->moves_.size(); i++) {
                const Piece& p = this->moves_[i];
                // this->update_board_with_piece(board, p);  // search the whole board
                const Piece& last_p = this->moves_[i - 1];
                board[last_p.row][last_p.col] = last_p.get_real_color();
                board[last_piece.row][last_piece.col] = last_piece.get_real_color() + 1;
                this->frames_.emplace_back(board);
            }
        }
        assert(this->moves_.size() == this->frame_.size());
    }
    void update_last_move() {
        assert(this->frames_.size() >= 2);
        const Tbl_type& last_board = this->frames_[this->frames_.size() - 2].get_tbl();
        const Tbl_type& new_board = this->frames_.back().get_tbl();
        bool found_sp_flag = false;
        for (int i = 0; i < Size; i++) {
            for (int j = 0; j < Size; j++) {
                if (last_board[i][j] == 0 && new_board[i][j] != 0) {
                    if (found_sp_flag) {
                        log_error("Multiple sp pieces found in update_last_move()");
                        log_error(XQ4GB_TIMESTAMP, "frames_ may has been broken");
                    } else {
                        this->moves_.emplace_back(i, j, Piece::Color{new_board[i][j]});
                        found_sp_flag = true;
                    }
                }
            }
        }
        if (!found_sp_flag) { log_error("No sp piece found in update_last_move"); }
        assert(moves_.size() == this->frames_.size());
    }
    std::vector<Piece> moves_;
};  // endof class ArchiveLight

}  // endof namespace mfwu

#endif  // __ARCHIVE_HPP__