#include "5-1_lexer.hpp"

namespace briqs {

    Lexer::Lexer(const std::stringstream& ss, Baseplate* p)
        : plate(p)
        , current_char(-1)
        , current_state(NORMAL)
        , finished(false)
        , tokn_type(ToknType::INVL)
        , tokn_name("") {

        input << ss.rdbuf();
        consume_char();
    }

    Tokn *Lexer::get_next_token() {
        while (!finished) {
            scan(current_state, current_char);
        }
        Tokn *new_token = plate->make<Tokn>(tokn_type, tokn_name);

        finished = false;
        tokn_name = "";

        return new_token;
    }

    void Lexer::consume_char() { current_char = input.get(); }

    void Lexer::scan(State s, char c) {
        switch (s) {
            case NORMAL:
            if (is_whitespace(c)) { ; }
            else if (c == '(') { finish(ToknType::LBCT, "("); }
            else if (c == ')') { finish(ToknType::RBCT, ")"); }
            else if (c == '"') { finish(ToknType::DBQT, "\"", STRING); }
            else if (c == '\'') { finish(ToknType::SGQT, "'"); }
            else if (c == ';') { current_state = COMMNT; }
            else if (c == EOF) { finish(ToknType::EOF_, "EOF_"); return; }
            else { current_state = SYMBOL; tokn_name += c; }
            consume_char();
            break;

            case SYMBOL:
            if (is_whitespace(c)) {
                consume_char();
                finish(ToknType::TOKN, tokn_name);
            } else if (c == '(' || c == ')' || c == '"' || c == EOF) {
                finish(ToknType::TOKN, tokn_name);
            } else { tokn_name += c; consume_char(); }
            break;

            case STRING:
            if (c == EOF) finish(ToknType::TOKN, tokn_name);
            else if (c == '"') {
                finish(ToknType::TOKN, tokn_name, STREND);
            } else { tokn_name += c; consume_char(); }
            break;

            case STREND:
            finish(ToknType::DBQT, "\"");
            consume_char();
            break;

            case COMMNT:
            if (c == EOF) {
                finished = true; current_state = NORMAL; consume_char();
            }
            else if (c == '\n') {
                current_state = NORMAL;
                consume_char();
            } else { consume_char(); }
            break;

            default:
            break;
        }
    }

    void Lexer::finish(ToknType t, std::string n, State next) {
        finished = true;
        tokn_type = t;
        tokn_name = n;
        current_state = next;
    }

    bool is_whitespace(char c) {
        return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
    }
} // namespace briqs
