/*
    Copyright (c) 2002-2015 Tampere University of Technology.

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
 * @file FUGenerator.hh
 *
 * Declaration of FUGenerator class.
 *
 * @author Nicolai Behmann 2015 (behmann-no.spam-ims.uni-hannover.de)
 * @note rating: red
 */
 
#ifndef _FU_GENERATOR_HPP_
#define _FU_GENERATOR_HPP_

#include <string>
#include <fstream>

#include "Machine.hh"

class FUGenerator {
private:
    TTAMachine::FunctionUnit* m_functionUnit;

    std::ofstream *m_ofstream;

    std::string m_strPackage_opcodes;
    std::string m_strEntity_arithm;
    
    std::string m_filenameVHDL;

    unsigned int m_nbitOpcode;
    
    std::vector<std::string> binOperations;

    int generateVHDL_opcode(void);
    int generateVHDL_arithm(void);
    int generateVHDL_regs(void);
protected:


public:
    FUGenerator(TTAMachine::FunctionUnit*);
    ~FUGenerator();

    int generateVHDL(std::string);
    
    int addFUImplementationToHDB(std::string);
};

#endif // #ifndef _FU_GENERATOR_HPP_
