#include "6_uuid.hpp"
#include <uuid/uuid.h>

namespace briqs {
    Uuid::Uuid() {
        uuid_generate(bval);
    }

    Uuid::Uuid(uuid_t uuid_from_bytes) {
        memcpy(bval, (byte *)uuid_from_bytes, UUID_SIZE);
    }

    std::string Uuid::info() const {
        uuid_string_t uuid_str;
        uuid_unparse_upper(bval, uuid_str);
        std::string uuid_std_string { uuid_str };

        return "UUID<" + uuid_std_string.substr(0, 8) + ">";
    }

    std::unique_ptr<byte> Uuid::cast_to_data() {
        byte* raw_data = new byte[TILE_SIZE]{ VCTR, UUID, };

        copy_to_byte(raw_data + 8, UUID_SIZE);
        memcpy(raw_data + 16, (byte *)bval, TILE_CNTT_SIZE);

        std::unique_ptr<byte> data(raw_data);
        return data;
    }

    Uuid* cast_to_uuid(Bucket* bucket, byte* bytes) {
        return new Uuid(bytes + 16);
    }

    void UuidScope::set_symbol_table() { // log("UuidScope::"); PRINT_FUNC_NAME
        symbol_table["uuid"] = new Prim(make_uuid);

        unpacking_traits[UUID] = cast_to_uuid;
    }

    Briq* make_uuid(Stiq* stiq, Briq* args) {
        return stiq->get_plate()->make<Uuid>();
    }
}
