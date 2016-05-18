#include "5-4_core_operators.hpp"

namespace briqs {

    void CoreOperatorScope::set_symbol_table() { // log("CoreOperatorScope::"); PRINT_FUNC_NAME
        symbol_table["Q"] = new Prim(quote, BriqType::SPFM);
        symbol_table[":"] = new Prim(define_symbol, BriqType::SPFM);
        symbol_table["?"] = new Prim(cond, BriqType::SPFM);
        symbol_table["^"] = new Prim(lambda, BriqType::SPFM);

        symbol_table["$"] = new Prim(syntax, BriqType::SPFM);

        symbol_table["@"] = new Prim(atom);
        symbol_table["="] = new Prim(eq);
        symbol_table["~"] = new Prim(cons);
        symbol_table["L"] = new Prim(car);
        symbol_table["G"] = new Prim(cdr);

        symbol_table["list"] = new Prim(list);
        symbol_table["begin"] = new Prim(begin);
    }


    // Special Forms
    Briq* quote(Stiq* stiq, Briq* briq) {
        return briq->l();
    }

    // TODO: need environment for this method
    Briq* define_symbol(Stiq* stiq, Briq* args) {

        Briq *sym = args->l();

        Briq *cnt = eval(stiq, args->g()->l());

        stiq->get_scope_stack_top()->define(sym->to_s(), cnt);

        return sym;
    }

    // TODO: should decide how evaluate first argument to Bool*
    Briq* cond(Stiq* stiq, Briq* args) {
        Briq* result = none;

        Briq* p = eval(stiq, args->l());

        if (p != none && p != fval && !(!p->is_atom() && !p->l())) {
            result = eval(stiq, args->g()->l());

        } else {
            if (args->g()->g()) {
                result = eval(stiq, args->g()->g()->l());
            }
        }
        return result;
    }

    // TODO: should decide internal specification of 'lambda form'
    Briq *lambda(Stiq* stiq, Briq* args) {
        Briq* result = none;

        if (args->g()) {
            Briq *parameters = args->l();
            result = stiq->get_plate()->make<Func>(args->l(), args->g());

        } else {
            log("!!!! INVL ARGS LMBD !!!!");
        }

        return result;
    }

    Briq* syntax(Stiq* stiq, Briq *args) {
        Briq* result = none;

        if (args->g()) {
            Briq *parameters = args->l();
            result = stiq->get_plate()->make<Sntx>(args->l(), args->g()->l());

        } else {
            log("!!!! INVL ARGS SNTX !!!!");
        }

        return result;
    }


    // functions
    Bool* atom(Stiq* stiq, Briq* args) {
        return btob(is_atom(args->l()));
    }

    Briq* cons(Stiq* stiq, Briq* args) {

        auto new_cell = stiq->get_plate()->make<Cell>();

        new_cell->set_lptr(args->l());
        new_cell->set_gptr(args->g()->l());

        return new_cell;
    }

    Briq* car(Stiq* stiq, Briq* args) {
        Briq *result = none;

        if (!is_null(args->l())) {
            result = args->l()->l();

        } else {
            result = none;
        }

        return result;
    }

    Briq* cdr(Stiq* stiq, Briq* args) {
        Briq *result = none;

        if (!is_null(args->l())) {
            result = args->l()->g();

        } else {
            result = none;
        }

        return result;
    }

    // TODO: should make function resulting not 'Bool*' but 'bool'?
    // TODO: should re-think equality of briq
    Briq* eq(Stiq* stiq, Briq *args) {
        Briq *result = none;

        Briq *arg1 = args->l();
        Briq *arg2 = args->g()->l();

        if (arg1 == fval || arg1 == tval) {
            result = btob(arg1 == arg2);

        } else if (arg1->type() == BriqType::TEXT || arg1->type() == BriqType::SMBL) {
            result = btob(arg1->to_s() == arg2->to_s());

        } else {
            result = fval;
        }

        return result;
    }


    // Other
    Briq* list(Stiq* stiq, Briq* list) {
        return list;
    }

    Briq* begin(Stiq* stiq, Briq* seq_list) {
        Briq* result = none;

        Briq* statement = seq_list;

        while (!is_null(statement)) {

            if (is_null(statement->g())) {
                result = statement->l();
            }

            statement = statement->g();
        }

        return result;
    }

} // namespace briqs
