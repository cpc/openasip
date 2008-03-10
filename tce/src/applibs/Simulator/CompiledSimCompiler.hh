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

class CompiledSimCodeGenerator;

class CompiledSimCompiler {
public:
    CompiledSimCompiler();
    ~CompiledSimCompiler();
    
    int compileDirectory(
        const std::string& dirName, 
        const std::string& flags = "-O3") const;
    
private:
    /// Copying not allowed.
    CompiledSimCompiler(const CompiledSimCompiler&);
    /// Assignment not allowed.
    CompiledSimCompiler& operator=(const CompiledSimCompiler&); 
};

#endif
