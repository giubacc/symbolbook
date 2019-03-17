/* Original Work Copyright (c) 2019 Giuseppe Baccini - giuseppe.baccini@live.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "str_tok.h"

namespace utl {

static const char *def_delims = " \t\n\r\f";

str_tok::str_tok(const std::string &str)  :
    current_position_(0),
    max_position_((long)str.length()),
    new_position_(-1),
    str_(str),
    delimiters_(def_delims),
    ret_delims_(false),
    delims_changed_(false)
{
}

str_tok::~str_tok()
{}

long str_tok::skip_delimit(long start_pos)
{
    long position = start_pos;
    while(!ret_delims_ && position < max_position_) {
        if(delimiters_.find(str_.at(position)) == std::string::npos) {
            break;
        }
        position++;
    }
    return position;
}

long str_tok::scan_token(long start_pos)
{
    long position = start_pos;
    while(position < max_position_) {
        if(delimiters_.find(str_.at(position)) != std::string::npos) {
            break;
        }
        position++;
    }
    if(ret_delims_ && (start_pos == position)) {
        if(delimiters_.find(str_.at(position)) != std::string::npos) {
            position++;
        }
    }
    return position;
}

bool str_tok::next_token(std::string &out,
                         const char *delimiters,
                         bool return_delimiters)
{
    if(delimiters) {
        delimiters_.assign(delimiters);
        delims_changed_ = true;
    }
    ret_delims_ = return_delimiters;
    current_position_ = (new_position_ >= 0 && !delims_changed_) ?
                        new_position_ :
                        skip_delimit(current_position_);
    delims_changed_ = false;
    new_position_ = -1;
    if(current_position_ >= max_position_) {
        return false;
    }
    long start = current_position_;
    current_position_ = scan_token(current_position_);
    out = str_.substr(start, current_position_ - start);
    return true;
}

bool str_tok::has_more_tokens(bool return_delimiters)
{
    ret_delims_ = return_delimiters;
    new_position_ = skip_delimit(current_position_);
    return (new_position_ < max_position_);
}

}
