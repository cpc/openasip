/*
 * Copyright (c) 2002-2017 Tampere University.
 *
 * This file is part of TTA-Based Codesign Environment (TCE).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * @file AssemblyParserDiagnostic.hh
 *
 * Declaration of assembly parser diagnostics.
 *
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef ASSEMBLY_PARSER_DIAGNOSTIC_HH
#define ASSEMBLY_PARSER_DIAGNOSTIC_HH

#include <string>
#include <memory>
#include <set>
#include <tuple>

#include "ParserStructs.hh"

/**
 * Struct for parser reports.
 */
struct CompilerMessage {
    std::string message = "";       ///< Message.
    std::string assemblerLine = ""; ///< Assembly code line number.
    UValue lineNumber = 0;          ///< Message generation line number.

    std::string toString() const {
        return std::string("Warning in line ") + std::to_string(lineNumber)
            + ": " + assemblerLine + "\n"
            + "reason: " + message;
    }

    bool operator<(const CompilerMessage& other) const {
        auto mine = std::tie(lineNumber, assemblerLine, message);
        auto theirs = std::tie(
            other.lineNumber, other.assemblerLine, other.message);
        return mine < theirs;
    }
};

/**
 * The class for storing and query assembly parser reports.
 */
class AssemblyParserDiagnostic {
public:
    void reset(std::shared_ptr<const std::string> assemblyText);

    void addWarning(UValue lineNumber, const std::string& message);
    void addError(UValue lineNumber, const std::string& message);
    void addError(const std::string& message);

    void clear();

    const std::set<CompilerMessage>& warnings() const {
        return warnings_;
    }

    bool anyErrors() const {
        return errors().size() && otherErrors().size();
    }

    const std::set<CompilerMessage>& errors() const {
        return codeErrors_;
    }

    const std::set<CompilerMessage>& otherErrors() const {
        return otherErrors_;
    }

    std::string codeLine(UValue lineNumber) const;

private:

    /// Warning messages.
    std::set<CompilerMessage> warnings_;

    /// Error messages with line number.
    std::set<CompilerMessage> codeErrors_;

    /// For positionless and internal errors.
    std::set<CompilerMessage> otherErrors_;

    /// The current assembly listing.
    std::shared_ptr<const std::string> listing_ = nullptr;

    /// New line start positions in assembly listing.
    std::vector<UValue> lineStarts_;
};

#endif // ASSEMBLY_PARSER_DIAGNOSTIC_HH
