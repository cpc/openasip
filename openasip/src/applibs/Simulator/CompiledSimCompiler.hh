/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file CompiledSimCompiler.hh
 *
 * Declaration of CompiledSimCompiler class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_COMPILER_HH
#define COMPILED_SIM_COMPILER_HH

#include <string>

/**
 * A class for compiling the dynamic libraries used by the compiled simulator
 */
class CompiledSimCompiler {
public:
    CompiledSimCompiler();
    virtual ~CompiledSimCompiler();
    
    int compileDirectory(
        const std::string& dirName,
        const std::string& flags = "",
        bool verbose = false) const;

    int compileFile(
        const std::string& path,
        const std::string& flags = "",
        const std::string& outputExtension = ".o",
        bool verbose = false) const;
    
    int compileToSO(
        const std::string& path,
        const std::string& flags = "",
        bool verbose = false) const;
    
    /// cpp flags used for compiled simulation
    static const char* COMPILED_SIM_CPP_FLAGS;
    
    /// flags used when compiling .so files
    static const char* COMPILED_SIM_SO_FLAGS;
    
private:
    /// Copying not allowed.
    CompiledSimCompiler(const CompiledSimCompiler&);
    /// Assignment not allowed.
    CompiledSimCompiler& operator=(const CompiledSimCompiler&);
    
    /// Number of threads to use while compiling through a Makefile
    int threadCount_;
    /// The compiler to use
    std::string compiler_;
    /// Global compile flags (from env variable)
    std::string globalCompileFlags_;
};

#endif
