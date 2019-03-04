#pragma once
#include <string>

namespace utl {

/** @brief string tokenizer class.
*/
struct str_tok {
        explicit str_tok(const std::string &str);

        ~str_tok();

        bool next_token(std::string &out,
                        const char *delimiters = nullptr,
                        bool return_delimiters = false);

        bool has_more_tokens(bool return_delimiters = false);

        void reset() {
            current_position_ = 0;
        }

    private:
        long skip_delimit(long start_pos);
        long scan_token(long start_pos);

    private:
        long current_position_, max_position_, new_position_;
        const std::string &str_;
        std::string delimiters_;
        bool ret_delims_, delims_changed_;
};

}

