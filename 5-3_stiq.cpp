#include "5-3_stiq.hpp"

namespace briqs {

    Scope::Scope()
        : name("")
        , enclosing_scope(nullptr) { // log("Scope::");PRINT_FUNC_NAME

        set_symbol_table();
    }

    Scope::Scope(std::string n, Scope *parent)
        : name(n)
        , enclosing_scope(parent) {

        set_symbol_table();
    }

    std::string Scope::get_scope_name() { return name; }

    Scope* Scope::get_enclosing_scope() { return enclosing_scope; }

    void Scope::define(std::string sym, Briq* content) { symbol_table[sym] = content; }

    Briq* Scope::resolve(std::string name) {
        Briq* result; // print_symbol_table();
        result = symbol_table[name];
        if (result) return result;
        if (enclosing_scope) return enclosing_scope->resolve(name);
        std::cout << "!!!! CANT RSLV !!!! NAME: [" << name << "]" << std::endl;
        return nullptr;
    }

    void Scope::set_symbol_table() { PRINT_FUNC_NAME
        symbol_table[" "] = new Prim(top_level, BriqType::PRIM, "<root>");
    }

    void Scope::print_symbol_table() {
        for (auto itr = symbol_table.begin(); itr != symbol_table.end(); ++itr) {
            std::cout << "key = [" << itr->first << "]" << std::endl;
        }
    }


    Stiq::Stiq(const std::stringstream& ss, Baseplate* p, Scope* scope)
        : plate(p) {

        parser = new Parser(new Lexer(ss, p), p);

        unpacking_traits[SBL_] = cast_to_smbl;

        auto global_scope = new Scope("global", scope);
        scope_stack.push(global_scope);
    }

    Stiq::~Stiq() {
        delete parser;
        delete plate;
    }


    Briq* Stiq::evaluate() {
        depth = -1;
        Briq* root = parser->parse();
        return eval(root->l());
        /*
        Briq* result = plate->make<Cell>();
        depth = -1;
        Briq* root = parser->parse();

        result->set_lptr(eval(root->l()));
        result->set_gptr(none);
        return result;
        */
    }


    Briq* Stiq::eval(Briq* briq) {
        return ::briqs::eval(this, briq);
    }

    Briq* eval(Stiq* stiq, Briq* briq) {
        Briq* result = nullptr;

        stiq->incr_depth();

        indented_eval_log(stiq->get_depth(), "EVAL BEGN ", briq);

        if (briq->is_self_evaluating()) {
            result = briq;

        } else if (briq->type() == BriqType::SMBL) {
            result = stiq->resolve_symbol(briq);

        } else if (!briq->is_atom()) {

            Briq *ope = eval(stiq, briq->l());

            if (is_null(ope)) {
                std::string indent(stiq->get_depth() * 2, ' ');
                log(indent + "apply operator is none!");
                result = none;

            } else {

                switch (ope->type()) {

                    case BriqType::SPFM:
                        result = (*ope)(stiq, briq->g());
                        break;

                    case BriqType::SNTX: {
                        Briq* expanded = stiq->exec_sntx(ope, briq->g());
                        result = eval(stiq, expanded);
                    }   break;

                    default: {
                        Briq* args = stiq->list_of_values(briq->g());
                        result = stiq->apply(ope, args);
                    }   break;
                }

            }
        }

        indented_eval_log(stiq->get_depth(), "EVAL FNSH ", result);

        stiq->decr_depth();

        return result;
    }

    Briq* map(Stiq* stiq, Briq* old_list, std::function<Briq*(Stiq*, Briq*)> f) {
        Briq* new_list_head = none;
        Briq* new_list_tail = none;

        auto old_cell = old_list;

        while (!is_null(old_cell)) {
            auto new_cell = stiq->make_list_item(f(stiq, old_cell->l()));

            if (is_null(new_list_tail)) {
                new_list_head = new_cell;
                new_list_tail = new_list_head;
            } else {
                new_list_tail->set_gptr(new_cell);
                new_list_tail = new_cell;
            }

            old_cell = old_cell->g();
        }

        return new_list_head;
    }

    Briq* top_level(Stiq* stiq, Briq* old_list) {
        return map(stiq, old_list, [](Stiq*, Briq* element){ return element; });
    }

    Briq* Stiq::resolve_symbol(Briq *smbl) {
        std::string symbol_name = smbl->to_s();
        return scope_stack.top()->resolve(symbol_name);
    }

    Briq* Stiq::list_of_values(Briq* old_list) {
        return ::briqs::list_of_values(this, old_list);
    }

    Briq* list_of_values(Stiq* stiq, Briq* old_list) {
        return map(stiq, old_list, [](Stiq* stiq, Briq* element){ return eval(stiq, element); });
    }

    Briq* Stiq::apply(Briq* proc, Briq* args) {
        switch (proc->type()) {
            case BriqType::PRIM: return (*proc)(this, args);
            case BriqType::FUNC: return exec_func(proc, args);
            default:             return none;
        }
    }

    Briq* Stiq::exec_func(Briq *lmbd, Briq *args) {
        Briq *result = none;

        Briq *params = lmbd->l();
        Briq *lambda_body = lmbd->g();

        scope_stack.push(new Scope("FuncScope", get_scope_stack_top()));

        destructuring_bind(params, args);

        result = sequence(lambda_body);

        Scope *top = scope_stack.top();
        delete top;
        scope_stack.pop();

        return result;
    }

    Briq* Stiq::exec_sntx(Briq *sntx, Briq *args) {
        Briq *result = none;

        Briq *params = sntx->l();
        Briq *lambda_body = sntx->g();

        destructuring_bind(params, args);

        result = eval(lambda_body);

        return result;
    }


    Briq* Stiq::sequence(Briq *seq_list) {
        return ::briqs::sequence(this, seq_list);
    }

    Briq* sequence(Stiq* stiq, Briq *seq_list) {
        Briq* result = none;

        Briq* statement = seq_list;

        while (!is_null(statement)) {

            if (!is_null(statement->g())) {
                eval(stiq, statement->l());
            } else {
                result = eval(stiq, statement->l());
            }

            statement = statement->g();
        }

        return result;
    }


    void Stiq::destructuring_bind(Briq* params, Briq* args) {
        ::briqs::destructuring_bind(this, params, args);
    }

    void destructuring_bind(Stiq* stiq, Briq* params, Briq* args) {
        Briq* args_current = args;

        while (!is_null(params)) {
            Briq* param_smbl = params->l();
            std::string symbol_name = param_smbl->to_s();

            if (symbol_name == ".") {

                // &rest or &body
                params = params->g();
                stiq->get_scope_stack_top()->define(params->l()->to_s(), args_current);
            } else {

                Briq* resolved_param = args_current->l();
                stiq->get_scope_stack_top()->define(symbol_name, resolved_param);
            }

            params = params->g();
            args_current = args_current->g();
        }
    }

} // namespace briqs
