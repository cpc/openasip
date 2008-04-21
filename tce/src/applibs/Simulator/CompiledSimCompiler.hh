/**
 * @file CompiledSimCompiler.hh
 *
 * Declaration of CompiledSimCompiler class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
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
        const std::string& flags = "-O0",
        bool verbose = false) const;
    
    int compileFile(
        const std::string& filePath,
        const std::string& flags = "-O0") const;
    
private:
    /// Copying not allowed.
    CompiledSimCompiler(const CompiledSimCompiler&);
    /// Assignment not allowed.
    CompiledSimCompiler& operator=(const CompiledSimCompiler&);
    
    /// Number of threads to use while compiling through a Makefile
    int threadCount_;
    /// The compiler to use
    std::string compiler_;
};

#endif
