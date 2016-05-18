#ifndef QUERY_HPP
#define QUERY_HPP

#include "3_facade.hpp"

namespace briqs {
    enum class ToknType { INVL, TOKN = 0x30, LBCT, RBCT, SGQT, DBQT, EOF_, };


    class Tokn : public Briq {
    public:
        Tokn(ToknType t, std::string n) : token_tp(t), name(n) {}

        BriqKind kind() const override { return BriqKind::STIQ; }
        BriqType type() const override { return BriqType::TOKN; }

        bool is_atom() const override { return true; }

        std::string info() const override { return "TOKN<" + name + ">"; }
        std::string to_s() const override { return name; }

        ToknType token_type() { return token_tp; }

    private:
        ToknType token_tp;
        std::string name;
    };


    class Smbl : public Briq {
    public:
        Smbl(std::string n) : name(n) {}

        BriqKind kind() const override { return BriqKind::STIQ; }
        BriqType type() const override { return BriqType::SMBL; }

        bool is_atom() const override { return true; }

        std::string info() const override { return "SMBL[" + name + "]"; }
        std::string to_s() const override { return name; }

        std::unique_ptr<byte> cast_to_data() override;

        bool is_self_evaluating() const override { return false; }

    private:
        std::string name;
    };


    Smbl* cast_to_smbl(Bucket* bucket, byte *bytes);


    class Stiq;
    class Prim : public Briq {
    public:
        Prim(std::function<Briq*(Stiq*, Briq*)> f) : p(f), tp(BriqType::PRIM), info_str("") {}
        Prim(std::function<Briq*(Stiq*, Briq*)> f, BriqType t) : p(f), tp(t), info_str("") {}
        Prim(std::function<Briq*(Stiq*, Briq*)> f, BriqType t, std::string info) : p(f), tp(t), info_str(info) {}

        BriqKind kind() const override { return BriqKind::STIQ; }
        BriqType type() const override { return tp; }

        bool is_atom() const override { return true; }

        std::string info() const override { return (info_str.size() == 0) ? "PRIM" : info_str; }

        Briq* operator()(Stiq* stiq, Briq* briq) override { return p(stiq, briq); }

    private:
        BriqType tp;
        std::function<Briq*(Stiq*, Briq*)> p;
        std::string info_str;
    };


    class Func : public Briq {
    public:
        Func(Briq* params, Briq* body) : func_params(params), func_body(body) {}

        BriqKind kind() const override { return BriqKind::STIQ; }
        BriqType type() const override { return BriqType::FUNC; }

        bool is_atom() const override { return true; }

        std::string info() const override { return "FUNC"; }

        Briq* l() override { return func_params; }
        Briq* g() override { return func_body; }

    private:
        Briq* func_params;
        Briq* func_body;
    };


    class Sntx : public Briq {
    public:
        Sntx(Briq* params, Briq* body) : sntx_params(params), sntx_body(body) {};

        BriqKind kind() const override { return BriqKind::STIQ; }
        BriqType type() const override { return BriqType::SNTX; }

        bool is_atom() const override { return true; }

        std::string info() const override { return "SNTX"; }

        Briq* l() override { return sntx_params; }
        Briq* g() override { return sntx_body; }

    private:
        Briq* sntx_params;
        Briq* sntx_body;
    };

} // namespace briqs

#endif // QUERY_HPP
