/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file CompileTools.hh
 *
 * Declaration of CompileTools class.
 *
 * Created on: 13.3.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef COMPILETOOLS_HH_
#define COMPILETOOLS_HH_

#include <string>
#include <sstream>

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
}

/*
 * Helper class to do in-code compiling.
 */
class CompileTools {
public:

    static TTAProgram::Program* compileAsC(
        const TTAMachine::Machine& target,
        std::stringstream& code,
        const std::string& cflags = "");
    static TTAProgram::Program* compileAsC(
        const TTAMachine::Machine& target,
        const std::string& code,
        const std::string& cflags = "");
    static TTAProgram::Program* compileAsLLVM(
        const TTAMachine::Machine& target,
        const std::stringstream& code,
        const std::string compileOptions);

private:
    CompileTools();
    virtual ~CompileTools();
};

#endif /* COMPILETOOLS_HH_ */
