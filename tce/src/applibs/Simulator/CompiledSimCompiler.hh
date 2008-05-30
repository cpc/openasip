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
