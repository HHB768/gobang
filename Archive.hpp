#ifndef __ARCHIVE_HPP__
#define __ARCHIVE_HPP__

namespace mfwu {
template <typename Seq_type>
class Archive {
public:
    void flush() {}
private:
    std::stack<Seq_type> this_game;
};  // endof class Archive

}  // endof namespace mfwu

#endif  // __ARCHIVE_HPP__