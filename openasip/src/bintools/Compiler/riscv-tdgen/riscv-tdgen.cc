/*
 Copyright (C) 2024 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
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

