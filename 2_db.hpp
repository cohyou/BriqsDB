#ifndef DB_HPP
#define DB_HPP

#include "1_gc.hpp"

namespace briqs {
    static const auto IN     = std::ios::in;
    static const auto OUT    = std::ios::out;
    static const auto BINARY = std::ios::binary;
    static const auto APP    = std::ios::app;

    template<typename T> char* to_bytes(T& i) { return static_cast<char *>(static_cast<void *>(&i)); }

    class Bucket {
    public:
        Bucket(std::string name) : bucket_name(name) {}


        std::string name() { return bucket_name; }


        bool operator==(Bucket another_bucket) { return bucket_name == another_bucket.bucket_name; }
        bool operator!=(Bucket another_bucket) { return !(*this == another_bucket); }
        bool operator< (Bucket another_bucket) { return bucket_name < another_bucket.bucket_name; }


        template<class T> Briq* save(T* briq);
        Briq* save_recursive(Briq* briq);
        Briq* load(briq_index index);
        void clear();

    private:
        std::string bucket_name;

        template<class T> std::fstream get_fstream(T* briq);

        void prepare();
        std::string get_file_path();
        briq_index get_max_id();
        briq_index incr_max_id();
    };


    class Dntr : public Sgfr {
    public:
        Dntr(Bucket* bc, briq_index idx) : bucket(bc), index(idx) {}

        SgfrType type() override { return SgfrType::DNTR; }

        Briq* get() override { return bucket->load(index); }


        bool operator==(Dntr another_dntr)
            { return (*bucket == *another_dntr.bucket) && (index == another_dntr.index); }

        bool operator<(const Dntr& another_dntr) const {
            if (*bucket != *another_dntr.bucket) {
                return *bucket < *another_dntr.bucket;
            } else {
                return index < another_dntr.index;
            }
        }


        std::string bucket_name() override { return bucket->name(); }

        void set_index(briq_index idx) override { index = idx; }

        briq_index get_index() override { return index; }

    private:
        Bucket* bucket;
        briq_index index;
    };


    extern std::map<int, std::function<Briq*(Bucket*, byte*)>> unpacking_traits;


    std::string string_from_tiles(Bucket* bucket, byte *bytes);

    
    template<class T>
    Briq* Bucket::save(T* briq) {

        if (briq->type() == BriqType::NONE) return briq;

        int requiredTileCount = briq->requiredTileCount();

        if (!briq->has_valid_index()) {
            for (int i = 0; i < requiredTileCount; ++i) {
                if (i == 0) {
                    briq->set_index(incr_max_id());
                } else {
                    incr_max_id();
                }
            }
        }

        auto fio = get_fstream(briq);

        std::unique_ptr<byte> data(briq->cast_to_data());

        fio.write(to_bytes(data.get()[0]), TILE_SIZE * requiredTileCount);

        return briq;
    }

    template<class T>
    std::fstream Bucket::get_fstream(T* briq) {
        std::fstream fio;

        briq->set_target_bucket(this->name());

        const char* path = get_file_path().c_str();

        if (briq->has_valid_index()) {
            // 既存
            fio = std::fstream(path, IN | OUT | BINARY);
            fio.seekp(briq->get_index() * TILE_SIZE, std::ios_base::beg);
        } else {
            // 新規
            fio = std::fstream(path, BINARY | APP);
        }

        return fio;
    }

} // namespace briqs

#endif // DB_HPP
