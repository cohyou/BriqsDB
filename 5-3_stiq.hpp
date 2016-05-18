#ifndef STIQ_HPP
#define STIQ_HPP

#include "5-2_parser.hpp"

namespace briqs {

    class Scope {
    public:
        Scope();
        Scope(std::string name, Scope *parent);

        std::string get_scope_name();
        Scope *get_enclosing_scope();
        void define(std::string sym, Briq *content);
        Briq *resolve(std::string name);

        virtual void set_symbol_table();

        void print_symbol_table();

    protected:
        std::map<std::string, Briq*> symbol_table;

    private:
        std::string name;
        Scope *enclosing_scope;
    };


    class Stiq {
    public:
        Stiq(const std::stringstream& ss, Baseplate* p, Scope* scope);
        ~Stiq();

        Briq* evaluate();

        Baseplate* get_plate() { return plate; }
        int get_depth() { return depth; }
        void incr_depth() { ++depth; }
        void decr_depth() { --depth; }
        Scope* get_scope_stack_top() { return scope_stack.top(); }

    public:
        Briq* resolve_symbol(Briq* smbl);
        Briq* list_of_values(Briq* l);
        Briq* apply(Briq* proc, Briq* args);

        Briq* exec_func(Briq *lmbd, Briq *args);
        Briq* exec_sntx(Briq *sntx, Briq *args);

    private:
        Briq* eval(Briq *briq);

        Briq* sequence(Briq *seq_list);

        void destructuring_bind(Briq* params, Briq* args);

    public:
        template <class T> Cell* make_list_item(T* briq);


    private:
        Parser* parser;
        Baseplate* plate;
        std::stack<Scope*> scope_stack;
        int depth;
    };


    Briq *eval(Stiq* stiq, Briq* briq);

    Briq* top_level(Stiq* stiq, Briq* old_list);

    Briq* sequence(Stiq* stiq, Briq *seq_list);

    Briq* list_of_values(Stiq* stiq, Briq* old_list);
    void destructuring_bind(Stiq* stiq, Briq* params, Briq* args);

    template <class T>
    Cell* Stiq::make_list_item(T* briq) {
        auto cell = plate->make<Cell>();
        cell->set_lptr(briq);
        cell->set_gptr(none);
        return cell;
    }

} // namespace briqs

#endif // STIQ_HPP
