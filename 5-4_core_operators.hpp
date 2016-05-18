#ifndef CORE_OPERATORS_HPP
#define CORE_OPERATORS_HPP

#include "5-3_stiq.hpp"

namespace briqs {

    class CoreOperatorScope : public Scope {
    public:
        CoreOperatorScope() : Scope() { set_symbol_table(); }
        void set_symbol_table() override;
    };

    Briq* quote(Stiq* stiq, Briq* briq);
    Briq* define_symbol(Stiq* stiq, Briq* args);
    Briq* cond(Stiq* stiq, Briq* args);
    Briq *lambda(Stiq* stiq, Briq* args);

    Briq* syntax(Stiq* stiq, Briq *args);

    Bool* atom(Stiq* stiq, Briq* args);
    Briq* cons(Stiq* stiq, Briq* args);
    Briq* car(Stiq* stiq, Briq* args);
    Briq* cdr(Stiq* stiq, Briq* args);
    Briq* eq(Stiq* stiq, Briq *args);

    Briq* list(Stiq* stiq, Briq* list);
    Briq* begin(Stiq* stiq, Briq* seq_list);

} // namespace briqs

#endif // CORE_OPERATORS_HPP
