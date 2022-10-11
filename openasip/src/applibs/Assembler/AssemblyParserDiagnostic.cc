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
 * Implementation of assembly parser diagnostics.
 *
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "AssemblyParserDiagnostic.hh"

/**
 * Resets assembly text. Also clears all reports.
 */
void
AssemblyParserDiagnostic::reset(
    std::shared_ptr<const std::string> assemblyText) {

    clear();

    listing_ = assemblyText;
    lineStarts_.push_back(0);
    for (size_t pos = 0; pos < listing_->size(); pos++) {
        char c = listing_->at(pos);
        if (c == '\n') {
            lineStarts_.push_back(pos+1);
        }
    }
}

void
AssemblyParserDiagnostic::addWarning(
    UValue lineNumber,
    const std::string& message) {

    CompilerMessage newWarning;
    newWarning.lineNumber = lineNumber;
    newWarning.message = message;
    newWarning.assemblerLine = codeLine(lineNumber);
    warnings_.insert(newWarning);
}

void
AssemblyParserDiagnostic::addError(
    UValue lineNumber, const std::string& message) {

    CompilerMessage msg;
    msg.lineNumber = lineNumber;
    msg.message = message;
    msg.assemblerLine = codeLine(lineNumber);
    codeErrors_.insert(msg);
}

void
AssemblyParserDiagnostic::addError(const std::string& message) {
    CompilerMessage msg;
    msg.message = message;
    otherErrors_.insert(msg);
}

/**
 * Clears all accumulated reports.
 */
void
AssemblyParserDiagnostic::clear() {
    warnings_.clear();
    codeErrors_.clear();
    otherErrors_.clear();
}

std::string
AssemblyParserDiagnostic::codeLine(UValue lineNumber) const {
    std::string errorLine;

    assert(listing_ && "reset() must be called before codeLine().");

    unsigned int errorLineNum =
        static_cast<unsigned int>(lineNumber - 1);

    if (errorLineNum < lineStarts_.size()) {

        std::string::size_type startPos =
            lineStarts_[errorLineNum];

        std::string::size_type endPos = listing_->find('\n', startPos);

        // no line feed at end of file
        if (endPos == std::string::npos) {
            endPos = listing_->length();
        }

        errorLine = listing_->substr(startPos, endPos - startPos);

    } else {
        errorLine = "Invalid line number info, probably last line of file.";
    }

    return errorLine;
}

