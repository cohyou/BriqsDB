#ifndef UUID_HPP
#define UUID_HPP

#include "5-5_functions.hpp"

namespace briqs {
    static const byte UUID_SIZE = 16;

    class Uuid : public Briq {
    public:
        Uuid();
        Uuid(uuid_t uuid_from_bytes);

        BriqKind kind() const override { return BriqKind::VCTR; }
        BriqType type() const override { return BriqType::UUID; }
        bool is_atom() const override { return true; }
        std::string info() const override;

        std::unique_ptr<byte> cast_to_data() override;
    private:
        uuid_t bval;
    };

    Uuid* cast_to_uuid(Bucket* bucket, byte* bytes);

    class UuidScope : public FunctionScope {
    public:
        UuidScope() : FunctionScope() { set_symbol_table(); }
        void set_symbol_table() override;
    };

    Briq* make_uuid(Stiq* stiq, Briq* args);
}

#endif
