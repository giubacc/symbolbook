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

inline std::string &ltrim(std::string &str, const std::string &chars = "\t\n\v\f\r ")
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

inline std::string &rtrim(std::string &str, const std::string &chars = "\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

inline std::string &trim(std::string &str, const std::string &chars = "\t\n\v\f\r ")
{
    return ltrim(rtrim(str, chars), chars);
}

inline std::string &find_and_replace(std::string &str, const char *find, const char *replace)
{
    size_t f_len = strlen(find), r_len = strlen(replace);
    for(std::string::size_type i = 0; (i = str.find(find, i)) != std::string::npos;) {
        str.replace(i, f_len, replace);
        i += r_len;
    }
    return str;
}

}

