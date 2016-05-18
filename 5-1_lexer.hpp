#ifndef LEXER_HPP
#define LEXER_HPP

#include "4_query.hpp"

namespace briqs {

    class Lexer {
    public:
        Lexer(const std::stringstream& ss, Baseplate* p);

        Tokn *get_next_token();
    private:
        enum State { NORMAL, SYMBOL, STRING, STREND, COMMNT, };


        void consume_char();
        void scan(State s, char c);
        void finish(ToknType t, std::string n, State next = NORMAL);


        Baseplate* plate;
        std::stringstream input;

        char current_char;
        State current_state;

        bool finished;
        ToknType tokn_type;
        std::string tokn_name;
    };

    bool is_whitespace(char c);
}

#endif
