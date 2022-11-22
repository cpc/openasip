#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

const int TEST_SAMPLES = 1023;
const std::string TCEASM = "limm_test.tceasm";

int main() {
    std::ofstream file;
    file.open(TCEASM.c_str());
    if (!file) {
        std::cerr << "Couldn't open file " << TCEASM << std::endl;
        return 1;
    }

    file << "CODE;" << std::endl << std::endl
         << ":global start;" << std::endl
         << "start:" << std::endl;

    int seed = time(NULL);
    srand(seed);

    file << "... , ... , ... [IU.0 = " << seed << "];" << std::endl
             << "IU.0 -> RF.0, ..., ...;" << std::endl;
    for (int i = 0; i < TEST_SAMPLES; i++) {
        int data = rand();
        if (rand()%2 == 0) {
            data *= -1;
        }
        file << "... , ... , ... [IU.0 = " << data << "];" << std::endl
             << "IU.0 -> RF.0, ..., ...;" << std::endl;
    }

    file.close();
    return 0;
}
        
