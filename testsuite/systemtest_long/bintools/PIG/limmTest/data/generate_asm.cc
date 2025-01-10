#include <cstdlib>
#include <iostream>
#include <fstream>
#include <time.h>

// Number of randomly generated values
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

    // Explicitly added immediate values

    // Regression test for case, where a value was falsely interpreted
    // as a negative value, when the MSB of a byte was '1'. Occurred,
    // when immediate unit used sign extension.
    file << "... , ... , ... [IU.0 = 136];" << std::endl
             << "IU.0 -> RF.0, ..., ...;" << std::endl;

    file.close();
    return 0;
}
        
