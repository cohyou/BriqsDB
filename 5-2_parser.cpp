#include "5-2_parser.hpp"

namespace briqs {

    Parser::Parser(Lexer* l, Baseplate* p)
        : lexer(l)
        , plate(p) {

        root = p->make<Cell>();
        root->set_gptr(none);
        current_node = root;

        auto smbl = plate->make<Smbl>(" ");
        add_child(smbl);

        consume_token();
    }

    Parser::~Parser() {
        delete lexer;
    }

    Briq* Parser::parse() {
        while (current_token->token_type() != ToknType::EOF_)
            { element(); }

        return root;
    }


    void Parser::element() { // PRINT_FUNC_NAME
        switch (current_token->token_type()) {
            case ToknType::TOKN:
            add();
            match(current_token->token_type());
            break;

            case ToknType::LBCT:
            list();
            break;

            case ToknType::SGQT:
            make_quoted_list();
            break;

            case ToknType::DBQT:
            text();
            break;

            default:
            std::cout << "!!!! SNTX ERRR !!!! [" << current_token->to_s() << "]" << std::endl;
            current_token = plate->make<Tokn>(ToknType::EOF_, "EOF_");
            break;
        }
    }


    void Parser::add() { // PRINT_FUNC_NAME
        std::string n = current_token->to_s();

        Briq* result = nullptr;

        std::vector<std::pair<std::function<bool(std::string)>, std::function<Briq*(std::string)>>> parse_pairs;

        auto f_p = [](std::string token_str) { return (token_str == "F"); };
        auto f_r = [](std::string token_str) { return fval; };
        parse_pairs.push_back(std::make_pair(f_p, f_r));

        auto t_p = [](std::string token_str) { return (token_str == "T"); };
        auto t_r = [](std::string token_str) { return tval; };
        parse_pairs.push_back(std::make_pair(t_p, t_r));

        auto n_p = [](std::string token_str) { return (token_str == "N"); };
        auto n_r = [](std::string token_str) { return none; };
        parse_pairs.push_back(std::make_pair(n_p, n_r));

        auto digit_p = [this](std::string token_str) { return is_digit(token_str); };
        auto digit_r = [this](std::string token_str) {
            unsigned long ul;
            std::istringstream iss(token_str);
            iss >> ul;
            return plate->make<Ui64>(ul);
        };
        parse_pairs.push_back(std::make_pair(digit_p, digit_r));

        
        for (auto parse_pair : parse_pairs) {
            if (parse_pair.first(n)) {
                result = parse_pair.second(n);
                break;
            }
        }

        if (!result) result = plate->make<Smbl>(n);

        add_child(result);
    }

    void Parser::list() { // PRINT_FUNC_NAME
        match(ToknType::LBCT);
        before();
        while (current_token->token_type() != ToknType::EOF_ &&
               current_token->token_type() != ToknType::RBCT) {
            element();
        }
        after();
        match(ToknType::RBCT);
    }

    void Parser::make_quoted_list() { // PRINT_FUNC_NAME
        match(ToknType::SGQT);
        before();
        add_child(plate->make<Smbl>("Q"));
        element();
        after();
    }

    void Parser::text() { // PRINT_FUNC_NAME
        match(ToknType::DBQT);
        Text *text = plate->make<Text>(current_token->to_s());
        add_child(text);
        match(ToknType::TOKN);
        match(ToknType::DBQT);
    }


    void Parser::before() {
        auto* new_cell = add_child(none);
        node_stack.push(current_node);
        current_node = new_cell;
    }

    void Parser::after() {
        current_node = node_stack.top();
        node_stack.pop();
    }


    Cell* Parser::add_child(Briq* briq) { // PRINT_FUNC_NAME

        auto new_cell = make_list_item(briq);            // log_debug("current_node: " + current_node->info() + "briq: " + briq->info());
        auto c = current_node->l();                      // log_debug("current_node->l(): " + c->info());

        if (is_null(c)) {                                // log_debug("c is null");
            current_node->set_lptr(new_cell);
        } else {                                         // log_debug("c is not null");
            while (c) {
                if (is_null(c->g())) {
                    c->set_gptr(new_cell);
                    break;
                }
                c = c->g();                              // indented_eval_log(0, "c = c->g(): ", c);
            }
        }                                                // log_debug("new_cell_last: " + c->info());

        return new_cell;
    }


    Cell* Parser::make_list_item(Briq* briq) {
        auto cell = plate->make<Cell>();
        cell->set_lptr(briq);
        cell->set_gptr(none);
        return cell;
    }


    void Parser::match(ToknType x) {
        if (current_token->token_type() == x) {
            consume_token();
        } else {
            std::cout << "!!!! SNTX ERRR !!!! (" << current_token->to_s() << ")" << std::endl;
        }
    }


    void Parser::consume_token() {
        current_token = lexer->get_next_token(); // log_debug(current_token->info());
    }


    bool Parser::is_digit(std::string n) {
        for (size_t i = 0; i < n.size(); ++i) {
            char c = n[i];
            if (i == 0 && n.size() > 1 && c == '0') return false;
            if (c == '0' || c == '1' ||c == '2' ||c == '3' ||c == '4' ||
                c == '5' || c == '6' ||c == '7' ||c == '8' ||c == '9') {
            } else {
                return false;
            }
        }

        return true;
    }

} // namespace briqs
