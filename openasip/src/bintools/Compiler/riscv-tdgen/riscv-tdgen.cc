/*
 Copyright (C) 2024-2026 Tampere University.

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */
/**
 * @file riscv-tdgen.cc
 *
 * LLVM target definition generator driver for RISC-V 
 *
 * @author Kari Hepola 2024 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <assert.h>
#include "RISCVTDGen.hh"
#include "Machine.hh"

/**
 * riscv-tdgen main function.
 *
 * Generates a RISC-V custom extension target definition file for LLVM
 */
int main(int argc, char* argv[]) {
    std::string outputDir;
    std::string adfPath;
    std::string rocc_str;
    bool rocc;

    // Check if the correct number of arguments is provided
    if (!(argc == 7 || argc == 5)) {
        std::cout << "Usage: riscv-tdgen" << std::endl
                  << "   -o Output directory." << std::endl
                  << "   -a ADF path." << std::endl
                  << "   -r 'T':Enable ROCC encodings, OR 'F'. Default:F" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse command-line arguments
    for (int i = 1; i < argc; i += 2) {
        if (std::strcmp(argv[i], "-o") == 0) {
            outputDir = argv[i + 1];
        } else if (std::strcmp(argv[i], "-a") == 0) {
            adfPath = argv[i + 1];
        } else if (std::strcmp(argv[i], "-r") == 0) {
            rocc_str = argv[i + 1];
        }
    }

    TTAMachine::Machine* mach = TTAMachine::Machine::loadFromADF(adfPath);
    assert(mach != NULL);

    if (rocc_str == "T") {
        rocc = true;
    } else {
        rocc = false;
    }

    RISCVTDGen tdgen(*mach, rocc);
    tdgen.generateBackend(outputDir);
    delete mach;

    return EXIT_SUCCESS;
}

