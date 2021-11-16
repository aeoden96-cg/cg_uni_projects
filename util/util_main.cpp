#include "util_main.hpp"
#include <string>

//using namespace std;

enum SHADER {
    VERTEX = 1,
    GEOMETRY = 2,
    TESSELATOR = 4 ,
    FRAGMENT = 8
};

class Util{
    bool readFile(char **lines, const std::string filename);

};

bool Util::readFile(char **lines, const std::string filename) {
    ofstream myfile;
    myfile.open (filename);
    myfile << "Writing this to a file.\n";
    myfile.close();
    return 0;
}
