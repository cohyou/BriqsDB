#ifndef GC_HPP
#define GC_HPP

#include "0_core.hpp"

namespace briqs {
    class Pile {
        std::vector<Briq*> briqs_;
    public:
        void add_briq(Briq* briq);
        ~Pile();
    };
} // namespace briqs

#endif // GC_HPP
