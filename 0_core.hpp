#ifndef CORE_HPP
#define CORE_HPP

#include <string>
#include <vector>
#include <map>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <fstream>
#include <cassert>
#include <sstream>
#include <stack>
#include <cstring>
#include <cstdlib>

namespace briqs {
    typedef unsigned char byte;
    typedef unsigned long briq_index;

    static const byte TILE_SIZE = 32;
    static const byte TILE_CNTT_SIZE = 16;

    enum class BriqKind { INVL, CODE, SVAL, VCTR, CELL, STIQ, };
    enum class BriqType { INVL, NONE, BOOL, UI64, TEXT, CELL, TOKN, SMBL, SPFM, PRIM, FUNC, SNTX,
                          UUID, };
    enum class SgfrType { INVL, DNTR, PNTR, };

    enum TileKind : byte { CODE = 0x01, SVAL, VCTR, CELL, STIQ, };
    enum TileType : byte { FVAL = 0xC0, TVAL = 0xC1, UI64 = 0xCF, TXT_ = 0xD9, TXT2 = 0xDA, CEL_ = 0x01, SBL_ = 0xE0, SBL2 = 0xE1,
                           UUID = 0xF0, };


    class Stiq;
    class Sgfr;
    class Briq {
    public:
        virtual BriqKind kind() const { return BriqKind::INVL; }
        virtual BriqType type() const { return BriqType::INVL; }


        virtual bool is_atom() const = 0;


        virtual std::string info() const = 0;
        virtual std::string to_s() const { return info(); }


        virtual operator bool() const { throw std::bad_cast(); }
        virtual operator unsigned long() const { throw std::bad_cast(); }
        virtual operator std::string() const { throw std::bad_cast(); }


        // virtual int size() const { return -1; }


        virtual SgfrType ltyp() const { return SgfrType::INVL; }
        virtual SgfrType gtyp() const { return SgfrType::INVL; }

        virtual void set_lptr(Briq* briq) {}
        virtual void set_gptr(Briq* briq) {}
        virtual void set_lsgr(Sgfr* sgfr) {}
        virtual void set_gsgr(Sgfr* sgfr) {}

        virtual Briq* l();
        virtual Briq* g();


        void set_target_bucket(std::string bucket_name) { target_bucket_name = bucket_name; }
        virtual std::unique_ptr<byte> cast_to_data() { return nullptr; }

        bool has_valid_index();
        virtual void set_index(briq_index idx);
        virtual briq_index get_index();
        virtual unsigned long requiredTileCount() { return 1; }


        virtual bool is_self_evaluating() const { return true; }
        virtual Briq* operator()(Stiq* stiq, Briq* b) { return nullptr; }


        virtual ~Briq() {}

    protected:
        std::string target_bucket_name;
        std::map<std::string, briq_index> denoter_infos;

        std::unique_ptr<byte> get_tiles_from_string(std::string str, byte kind, byte type1, byte type2);

    private:
        bool exists_in(std::string bucket_name);
        briq_index get_index_of(std::string bucket_name);
        void set_index_of(std::string bucket_name, briq_index index);
    };


    class None : public Briq {
    public:
        None() {};

        BriqKind kind() const override { return BriqKind::SVAL; }
        BriqType type() const override { return BriqType::NONE; }

        bool is_atom() const override { return true; }

        std::string info() const override { return "NONE"; }
        std::string to_s() const override { return "N"; }
    };

    extern None* none;


    class Bool : public Briq {
    public:
        Bool(bool b) : bval(b) {};

        BriqKind kind() const override { return BriqKind::SVAL; }
        BriqType type() const override { return BriqType::BOOL; }

        bool is_atom() const override { return true; }

        std::string info() const override { return bval ? "TVAL" : "FVAL"; }
        std::string to_s() const override { return bval ? "T" : "F"; }

        operator bool() const override { return bval; }

        std::unique_ptr<byte> cast_to_data() override;

    private:
        bool bval;
    };

    extern Bool* fval;
    extern Bool* tval;


    class Ui64 : public Briq {
    public:
        Ui64(unsigned long ul) : bval(ul) {};

        BriqKind kind() const override { return BriqKind::SVAL; }
        BriqType type() const override { return BriqType::UI64; }

        bool is_atom() const override { return true; }

        std::string info() const override { return std::to_string(bval); }

        operator unsigned long() const override { return bval; }

        std::unique_ptr<byte> cast_to_data() override;

    private:
        unsigned long bval;
    };


    class Text : public Briq {
    public:
        Text(std::string s) : bval(s) {};

        BriqKind kind() const override { return BriqKind::SVAL; }
        BriqType type() const override { return BriqType::TEXT; }

        bool is_atom() const override { return true; }

        std::string info() const override { return "Text\"" + bval + "\""; }
        std::string to_s() const override { return "\"" + bval + "\""; }

        // int size() const override { return bval.size(); }

        operator std::string() const override { return bval; }

        std::unique_ptr<byte> cast_to_data() override;

        unsigned long requiredTileCount() override {
            return ((bval.size() - 1) / TILE_CNTT_SIZE + 1);
        }

    private:
        std::string bval;
    };


    class Sgfr {
    public:
        virtual SgfrType type() { return SgfrType::INVL; }

        virtual Briq* get() { return nullptr; }

        bool operator==(Sgfr another_sgfr) { return false; }
        bool operator<(const Sgfr& another_sgfr) const { return false; }

        virtual std::string bucket_name() { return ""; }
        virtual void set_index(briq_index idx) {}
        virtual briq_index get_index() { return ULONG_MAX; }

        bool is_pntr() { return type() == SgfrType::PNTR; }
        bool is_dntr() { return type() == SgfrType::DNTR; }

        // virtual std::string info() const = 0;
    };


    class Pntr : public Sgfr {
    public:
        Pntr(Briq *briq) : pval(briq) {};

        SgfrType type() override { return SgfrType::PNTR; }

        Briq* get() override;

        // std::string info() const override { return "PNTR" }
    private:
        Briq* pval;
    };


    class Cell : public Briq {
    public:
        Cell();

        BriqKind kind() const override { return BriqKind::CELL; }
        BriqType type() const override { return BriqType::CELL; }

        bool is_atom() const override { return false; }

        std::string info() const override {
            return "CELL";
            /*
            std::stringstream ss;
            ss << "CELL{";
            switch (ltyp()) {
                case SgfrType::PNTR:
                case SgfrType::DNTR:
                case default:        break;
            }
            return ss.str();
            */
        }


        SgfrType ltyp() const override { return lsgr->type(); }
        SgfrType gtyp() const override { return gsgr->type(); }

        void set_pntr(std::unique_ptr<Sgfr>* old_sgfr, Briq* briq) {
            if ((*old_sgfr)->is_dntr() || (*old_sgfr)->get() != briq) {
                (*old_sgfr).reset(new Pntr(briq));
            }
        }
        void set_lptr(Briq* briq) override { set_pntr(&lsgr, briq); }
        void set_gptr(Briq* briq) override { set_pntr(&gsgr, briq); }

        void set_sgfr(std::unique_ptr<Sgfr>* old_sgfr, Sgfr* new_sgfr) {
            if ((*old_sgfr).get() != new_sgfr) {
                (*old_sgfr).reset(new_sgfr);
            }
        }
        void set_lsgr(Sgfr* sgfr) override { set_sgfr(&lsgr, sgfr); }
        void set_gsgr(Sgfr* sgfr) override { set_sgfr(&gsgr, sgfr); }

        Briq* get_pntr(std::unique_ptr<Sgfr>* sgfr);
        Briq* l() override { return get_pntr(&lsgr); }
        Briq* g() override { return get_pntr(&gsgr); }


        std::unique_ptr<byte> cast_to_data() override;


        bool is_self_evaluating() const override { return false; }

    private:
        std::unique_ptr<Sgfr> lsgr;
        std::unique_ptr<Sgfr> gsgr;

        void copy_sgfr_to_byte(void* data, Sgfr* sgfr);
    };


    void copy_to_byte(void* data, unsigned long ul);
    Bool* btob(bool b);
    bool is_atom(Briq* b);
    void msg_exit(std::string msg);

    bool is_null(Briq* briq);

    void print(Briq *briq);
    void print_tree(Briq *briq);

    void indented_eval_log(int depth, std::string message, Briq* briq);
    #define PRINT_FUNC_NAME log_debug(std::string("NOW: ") + __func__);
    void log_pntr(Briq* briq);
    void log_debug(std::string s);
    void log(std::string s);
    bool is_debugging();

} // namespace briqs

#endif // CORE_HPP
