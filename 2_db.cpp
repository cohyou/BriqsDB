#include "2_db.hpp"

namespace briqs {

    Bool* cast_to_bool(Bucket* bucket, byte* bytes) {
        return new Bool((bytes[1] == TVAL));
    }

    Ui64* cast_to_ui64(Bucket* bucket, byte* bytes) {
        unsigned long ul;
        memcpy(&ul, bytes + 16, sizeof(ul));
        return new Ui64(ul);
    }

    std::string string_from_tiles(Bucket* bucket, byte* bytes) {
        unsigned long text_size;
        memcpy(&text_size, bytes + 8, sizeof(text_size));

        std::stringstream ss;
        for (int i = 0; i < text_size; ++i) {
            ss << bytes[TILE_SIZE * (i / TILE_CNTT_SIZE) + (16 + i % TILE_CNTT_SIZE)];
        }
        return ss.str();
    }

    Text* cast_to_text(Bucket* bucket, byte* bytes) {
        return new Text(string_from_tiles(bucket, bytes));
    }

    void set_index_to_cell(Cell* cell, Bucket* bucket, byte* start, char l_or_g) {
        unsigned long index;

        memcpy(&index, start, sizeof(index));

        if (index == 0) {
            switch (l_or_g) {
                case 'L': cell->set_lptr(none); break;
                case 'G': cell->set_gptr(none); break;
                default: break;
            }
        } else {
            auto dntr = new Dntr(bucket, index);
            switch (l_or_g) {
                case 'L': cell->set_lsgr(dntr); break;
                case 'G': cell->set_gsgr(dntr); break;
                default: break;
            }
        }
    }

    Cell* cast_to_cell(Bucket* bucket, byte* bytes) {
        auto c = new Cell();

        set_index_to_cell(c, bucket, bytes + 16, 'L');
        set_index_to_cell(c, bucket, bytes + 24, 'G');

        return c;
    }

    std::map<int, std::function<Briq*(Bucket*, byte*)>> unpacking_traits =
    {
        {0x00, cast_to_cell},

        {FVAL, cast_to_bool},
        {TVAL, cast_to_bool},
        {UI64, cast_to_ui64},
        {TXT_, cast_to_text},
        {CEL_, cast_to_cell},
    };


    // template<> Briq* Bucket::save(None* n) { return n; }

    Briq* Bucket::save_recursive(Briq* briq) {
        assert(briq);

        if (briq->kind() == BriqKind::CELL) {
            Briq *l = briq->l();
            if (l != none) { save_recursive(l); }
            Briq *g = briq->g();
            if (g != none) { save_recursive(g); }
        }

        return save(briq);
    }

    Briq* Bucket::load(briq_index index) {
        Briq* briq = nullptr;
        byte data[TILE_SIZE] = {};

        std::ifstream fin(get_file_path().c_str(), BINARY);
        fin.seekg(TILE_SIZE * index, std::ios_base::beg);
        fin.read(to_bytes(data[0]), TILE_SIZE);

        if (data[1] == TXT_) {
            // for Text

            unsigned long text_size;
            memcpy(&text_size, data + 8, sizeof(text_size));

            int tile_count = ((text_size - 1) / TILE_CNTT_SIZE) + 1;
            int data_length = tile_count * TILE_SIZE;
            byte data_text[data_length];

            std::ifstream fin2(get_file_path().c_str(), BINARY);
            fin2.seekg(TILE_SIZE * index, std::ios_base::beg);

            fin2.read(to_bytes(data_text[0]), data_length);

            briq = unpacking_traits[data_text[1]](this, data_text);

        } else {
            briq = unpacking_traits[data[1]](this, data);
        }

        briq->set_target_bucket(this->name());
        briq->set_index(index);

        return briq;
    }

    void Bucket::clear() {
        std::ifstream fin(get_file_path().c_str());
        if (fin.is_open()) remove(get_file_path().c_str());
        fin.close();

        prepare();
    }

    void Bucket::prepare() {
        std::ifstream fin(get_file_path().c_str());
        if (!fin.is_open()) {
            std::ofstream fout(get_file_path().c_str(), BINARY | APP);
            char cs[TILE_SIZE] = {};
            fout.write(cs, TILE_SIZE);
        }
    }

    std::string Bucket::get_file_path() {
        return "buckets/" + bucket_name + ".bc";
    }

    briq_index Bucket::get_max_id() {
        briq_index mxid;

        std::fstream fio(get_file_path().c_str(), IN | OUT | BINARY);
        fio.seekp(24, std::ios_base::beg);
        fio.read(to_bytes(mxid), sizeof(mxid));

        return mxid;
    }

    briq_index Bucket::incr_max_id() {
        briq_index newmxid = get_max_id() + 1;

        std::fstream fio(get_file_path().c_str(), IN | OUT | BINARY);
        fio.seekp(24, std::ios_base::beg);
        fio.write(to_bytes(newmxid), sizeof(newmxid));

        return newmxid;
    }

}
