#ifndef PARSER_HPP
#define PARSER_HPP

#include "5-1_lexer.hpp"

namespace briqs {

    class Parser {
    public:
        Parser(Lexer* lexer, Baseplate* p);
        ~Parser();

        Briq* parse();

    private:
        void element();

        void add();
        void list();
        void make_quoted_list();
        void text();

        void before();
        void after();

        Cell* add_child(Briq* briq);
        Cell* make_list_item(Briq* briq);

        void match(ToknType x);

        void consume_token();

        bool is_digit(std::string n);

        Lexer *lexer;
        Baseplate* plate;

        Tokn* current_token;

        Briq* root;
        Briq* current_node;
        std::stack<Briq*> node_stack;
    };
    
} // namespace briqs

#endif // PARSER_HPP
