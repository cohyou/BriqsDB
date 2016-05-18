#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "5-4_core_operators.hpp"

namespace briqs {
    class FunctionScope : public CoreOperatorScope {
    public:
        FunctionScope() : CoreOperatorScope() { set_symbol_table(); }
        void set_symbol_table() override;
    };

    Briq* str(Stiq* stiq, Briq *args);
    Briq* to_s(Stiq* stiq, Briq *args);
    Briq* print_no_break(Stiq* stiq, Briq *args);
    Briq* print_break(Stiq* stiq, Briq *args);
    Briq* ln(Stiq* stiq, Briq *args);

    Briq* gt(Stiq* stiq, Briq *args);
    Briq* lt(Stiq* stiq, Briq *args);

    Briq* setl(Stiq* stiq, Briq* args);
    Briq* setg(Stiq *stiq, Briq *args);

    Briq* clear_bucket(Stiq *stiq, Briq *args);
    Briq* save(Stiq* stiq, Briq* args);
    Briq* load(Stiq* stiq, Briq *args);
    Briq* index(Stiq* stiq, Briq *args);
    Briq* save_recursive(Stiq* stiq, Briq *args);

    Briq* import(Stiq* stiq, Briq *args);
    Briq* export_(Stiq* stiq, Briq *args);

} // namespace briqs

#endif // FUNCTIONS_HPP
