#include "5-5_functions.hpp"

namespace briqs {
    void FunctionScope::set_symbol_table() { // log("FunctionScope::"); PRINT_FUNC_NAME
        symbol_table["string"] = new Prim(str);
        symbol_table["to_s"] = new Prim(to_s);
        symbol_table["print"] = new Prim(print_no_break);
        symbol_table["println"] = new Prim(print_break);
        symbol_table["ln"] = new Prim(ln);

        symbol_table["<"] = new Prim(lt);
        symbol_table[">"] = new Prim(gt);

        symbol_table["setl"] = new Prim(setl);
        symbol_table["setg"] = new Prim(setg);

        symbol_table["bucket"] = new Prim(clear_bucket);
        symbol_table["save"] = new Prim(save);
        symbol_table["load"] = new Prim(load);
        symbol_table["index"] = new Prim(index);
        symbol_table["save-recursive"] = new Prim(save_recursive);

        symbol_table["import"] = new Prim(import);
        symbol_table["export"] = new Prim(export_);
    }


    // util for string
    Briq* str(Stiq* stiq, Briq *args) {
        std::stringstream ss;

        Briq* arg_list = args;

        while (!is_null(arg_list)) {

            Briq* arg = arg_list->l();

            ss << static_cast<std::string>(*arg);

            arg_list = arg_list->g();
        }

        return stiq->get_plate()->make<Text>(ss.str());
    }

    Briq* to_s(Stiq* stiq, Briq *args) {
        Briq* target = args->l();
        return stiq->get_plate()->make<Text>(target->to_s());
    }

    Briq* print_no_break(Stiq* stiq, Briq *args) {
        Briq* target = args->l();
        std::cout << static_cast<std::string>(*target);
        return none;
    }

    Briq* print_break(Stiq* stiq, Briq *args) {
        Briq *target = args->l();
        std::cout << static_cast<std::string>(*target) << std::endl;
        return none;
    }

    Briq* ln(Stiq* stiq, Briq *args) {
        return stiq->get_plate()->make<Text>("\n");
    }


    // comparison
    Briq* gt(Stiq* stiq, Briq *args) {
        Briq *result = none;

        Briq *arg1 = args->l();
        Briq *arg2 = args->g()->l();

        if (arg1->type() == BriqType::TEXT) {
            result = btob(arg1->to_s() > arg2->to_s());
        }

        return result;
    }

    Briq* lt(Stiq* stiq, Briq *args) {
        Briq *result = none;

        Briq *arg1 = args->l();
        Briq *arg2 = args->g()->l();

        if (arg1->type() == BriqType::TEXT) {
            result = btob(arg1->to_s() < arg2->to_s());
        }

        return result;
    }


    // operation for cell
    Briq* setl(Stiq* stiq, Briq* args) {
        Briq* arg1 = args->l();
        Briq* arg2 = args->g()->l();

        arg1->set_lptr(arg2);

        return arg1;
    }

    Briq* setg(Stiq *stiq, Briq *args) {
        Briq* arg1 = args->l();
        Briq* arg2 = args->g()->l();

        arg1->set_gptr(arg2);

        return arg1;
    }


    // operation for db
    Briq* clear_bucket(Stiq *stiq, Briq *args) {
        Briq* bucket_text = args->l();

        stiq->get_plate()->clear_bucket(*bucket_text);

        return bucket_text; // plate->load_briq(0, bucket_text->name(), 0);
    }

    Briq* save(Stiq* stiq, Briq* args) {
        Briq *save_target = args->l();
        Briq *bucket_text = args->g()->l();

        return stiq->get_plate()->save_briq(save_target, *bucket_text);
    }

    Briq* load(Stiq* stiq, Briq *args) {
        Briq *briq_id = args->l();
        Briq *bucket_text = args->g()->l();

        return stiq->get_plate()->load_briq(*briq_id, *bucket_text);
    }

    Briq* index(Stiq* stiq, Briq *args) {
        Briq* arg1 = args->l();
        return stiq->get_plate()->make<Ui64>(arg1->get_index());
    }

    Briq* save_recursive(Stiq* stiq, Briq *args) {
        Briq *save_target = args->l();
        Briq *bucket_text = args->g()->l();

        return stiq->get_plate()->save_briq_recursive(save_target, *bucket_text);
    }


    // import export
    Briq* import(Stiq* stiq, Briq *args) {
        Briq* result = none;

        Briq* bucket_text = args->l();

        Briq* ent = stiq->get_plate()->load_briq(0, *bucket_text);

        Briq* statement = ent->l();
        result = statement;

        while (is_null(statement)) {

            result = eval(stiq, statement->l());

            statement = statement->g();
        }

        return result;
    }

    Briq* export_(Stiq* stiq, Briq *args) {
        Briq* result = none;

        Briq* bucket_text = args->l();
        Briq* contents = args->g()->l();

        stiq->get_plate()->clear_bucket(*bucket_text);

        Briq* ent = stiq->get_plate()->load_briq(0, *bucket_text);

        Briq* saved_contents = stiq->get_plate()->save_briq_recursive(contents, *bucket_text);

        ent->set_lptr(saved_contents);

        result = stiq->get_plate()->save_briq(ent, *bucket_text);

        return result;
    }

} // namespace briqs
