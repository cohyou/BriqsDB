#include "6_uuid.hpp"
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    try {
        std::string path = argv[1];

        setenv("DEBUGGING", "0", 1);
        if (argc >= 3) {
            std::string opt = argv[2];
            if (opt == "-d") {
                setenv("DEBUGGING", "1", 1);
            }
        }

        struct stat st;
        auto filename = path.c_str();
        if (stat(filename, &st) != 0) {
            std::cerr << "ERRR: file not exists." << std::endl;
            return 1;
        }

        std::fstream fs(filename);
        std::stringstream ss;
        ss << fs.rdbuf();

        briqs::Baseplate* p = new briqs::Baseplate();

        briqs::Scope* scope = new briqs::UuidScope();

        briqs::Stiq stiq(ss, p, scope);

        briqs::Briq* r = stiq.evaluate();

        print(r);
        std::cout << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
