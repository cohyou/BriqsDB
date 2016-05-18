#include "0_core.hpp"
//
namespace briqs {
    None* none = new None();
    Bool* fval = new Bool(false);
    Bool* tval = new Bool(true);


    Briq* Briq::l() { return none; }
    Briq* Briq::g() { return none; }

    bool Briq::has_valid_index() { return (exists_in(target_bucket_name)); }

    void Briq::set_index(briq_index idx) { set_index_of(target_bucket_name, idx); }

    briq_index Briq::get_index() { return get_index_of(target_bucket_name); }


    bool Briq::exists_in(std::string bucket_name) {
        return (denoter_infos.count(bucket_name) > 0);
    }

    briq_index Briq::get_index_of(std::string bucket_name) {
        return denoter_infos[bucket_name];
    }

    void Briq::set_index_of(std::string bucket_name, briq_index idx) {
        denoter_infos[bucket_name] = idx;
    }

    std::unique_ptr<byte> Briq::get_tiles_from_string(std::string str, byte kind, byte type1, byte type2) {
        int requiredTileCount = ((str.size() - 1) / TILE_CNTT_SIZE + 1);

        int requiredByteLength = requiredTileCount * TILE_SIZE;

        byte* raw_data = new byte[requiredByteLength]{kind};

        copy_to_byte(raw_data + 8, str.size());

        for (int i = 0; i < requiredTileCount; ++i) {
            raw_data[TILE_SIZE * i + 1] = (i == 0) ? type1 : type2;
            memcpy(raw_data + TILE_SIZE * i + 16, str.c_str() + TILE_SIZE * i, TILE_CNTT_SIZE);
        }

        std::unique_ptr<byte> data(raw_data);
        return data;
    }



    std::unique_ptr<byte> Bool::cast_to_data() {
        byte t = bval ? TVAL : FVAL;
        std::unique_ptr<byte> data(new byte[TILE_SIZE]{SVAL, t});
        return data;
    }



    std::unique_ptr<byte> Ui64::cast_to_data() {

        byte* raw_data = new byte[TILE_SIZE]{SVAL, UI64};

        copy_to_byte(raw_data + 16, bval);

        std::unique_ptr<byte> data(raw_data);
        return data;
    }



    std::unique_ptr<byte> Text::cast_to_data() {
        return get_tiles_from_string(bval, VCTR, TXT_, TXT2);
    }


    Briq* Pntr::get() { // PRINT_FUNC_NAME
        return pval;
    }


    Cell::Cell() : lsgr(new Pntr(none)), gsgr(new Pntr(none)) {};


    Briq* Cell::get_pntr(std::unique_ptr<Sgfr>* sgfr) {
        Briq* briq = (*sgfr)->get();                  // log_debug(std::string(__func__)+"1");
        if ((*sgfr)->is_dntr()) set_pntr(sgfr, briq); // log_debug(std::string(__func__)+"2"); log_pntr(briq);
        return briq;
    }


    std::unique_ptr<byte> Cell::cast_to_data() {
        byte* raw_data = new byte[TILE_SIZE]{CELL, CEL_};

        copy_sgfr_to_byte(raw_data + 16, &*lsgr);
        copy_sgfr_to_byte(raw_data + 24, &*gsgr);

        std::unique_ptr<byte> data(raw_data);
        return data;
    }


    void Cell::copy_sgfr_to_byte(void* data, Sgfr* sgfr) {
        unsigned long index;
        switch (sgfr->type()) {
            case SgfrType::PNTR: {
                Briq* pntr = sgfr->get();
                pntr->set_target_bucket(target_bucket_name);
                index = pntr->get_index();
            }   break;

            case SgfrType::DNTR:
                index = sgfr->get_index();
                break;

            default:
                break;
        }
        copy_to_byte(data, index);
    }


    void copy_to_byte(void* data, unsigned long ul) {
        memcpy(data, &ul, sizeof(ul));
    }

    Bool* btob(bool b) { return b ? tval : fval; }

    bool is_atom(Briq* b) { return b->is_atom(); }

    void msg_exit(std::string msg) {
        std::cerr << msg << std::endl;
        exit(1);
    }


    bool is_null(Briq* briq) { // PRINT_FUNC_NAME
        if (!briq) return true;
        return (briq == none || briq->type() == BriqType::NONE);
    }


    void print(Briq *briq) {

        if (briq->type() == BriqType::SMBL && briq->to_s() == " ") {
            std::cout << "<root>";

        } else if (briq->is_atom()) {
            std::cout << briq->to_s();

        } else {
            std::cout << "(";

            auto c = briq;
            while (!is_null(c)) {

                print(c->l());

                c = c->g();

                if (!is_null(c)) {

                    if (c->is_atom()) {
                        std::cout << " . ";
                        std::cout << c->to_s();
                        std::cout << ")";
                        return;
                    } else {
                        std::cout << " ";
                    }
                }
            }

            std::cout << ")";
        }
    }

    void print_tree(Briq *briq) {

        std::function<void(Briq*, int)> print_tree_internal = [&](Briq* b, int i) {
            std::string indent(i * 2, ' ');
            std::cout << indent << b->info() << std::endl;

            if (!b->is_atom()) {
                auto lptr = b->l();
                if (lptr) print_tree_internal(lptr, i + 1);

                auto gptr = b->g();
                if (gptr) print_tree_internal(gptr, i);
            }
        };

        print_tree_internal(briq, 0);
    }

    void indented_eval_log(int depth, std::string message, Briq* briq) {
        if (is_debugging()) {
            std::string indent(depth * 2, ' ');
            std::cout << indent << message; print(briq); std::cout << std::endl;
        }
    }
    void log_pntr(Briq* briq) { if (is_debugging()) std::cout << briq << std::endl; }
    void log_debug(std::string s) { if (is_debugging()) log(s); }
    void log(std::string s) { std::cout << s << std::endl; }
    bool is_debugging() { return strcmp(getenv("DEBUGGING"), "1") == 0; }

} // namespace briqs
