#include "4_query.hpp"

namespace briqs {

    std::unique_ptr<byte> Smbl::cast_to_data() {
        return get_tiles_from_string(name, STIQ, SBL_, SBL2);
    }


    Smbl* cast_to_smbl(Bucket* bucket, byte *bytes) {
        return new Smbl(string_from_tiles(bucket, bytes));
    }

} // namespace briqs
