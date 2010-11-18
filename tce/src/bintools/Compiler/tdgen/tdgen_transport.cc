/*
    Copyright (c) 2002-2010 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

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
 * LLVM backend generator for the transport style backend.
 *
 * @author Pekka Jääskeläinen 2010
 *
 * @note rating: red
 */
#include "TCEString.hh"
#include "TransportTDGen.hh"
#include "Machine.hh"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cout 
            << "Usage: tdgen_transport adf_file output_dir"
            << std::endl;
        return EXIT_FAILURE;
    }
    TTAMachine::Machine* machine = NULL;
    try {
        machine = TTAMachine::Machine::loadFromADF(TCEString(argv[1]));
    } catch (const Exception& e) {
        std::cerr
            << "Unable to load the ADF. "
            << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    std::string outDir = TCEString(argv[2]);
    try {
        TransportTDGen gen(*machine);
        gen.generateBackend(outDir);
    } catch (const Exception& e) {
        std::cerr
            << "Error: " << e.errorMessage() << std::endl;
#if 0
            << e.fileName() << ":" << e.procedureName() << ":"
            << e.lineNum() << std::endl;
#endif
        
        return EXIT_FAILURE;
    }

    delete machine; machine = NULL;
    return EXIT_SUCCESS;   
}
