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
 * @file OperationBuilder.cc
 *
 * Declaration of OperationBuilder class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_BUILDER_HH
#define TTA_OPERATION_BUILDER_HH

#include <string>
#include <vector>

/**
 * Class that handles building the operation behavior.
 */
class OperationBuilder {
public:
    static OperationBuilder& instance();

    std::string xmlFilePath(const std::string& xmlFile);
    std::string behaviorFile(
        const std::string& baseName,
        std::string& path);

    bool buildObject(const std::string& baseName,
                     const std::string& behaviorFile,
                     const std::string& path,
                     std::vector<std::string>& output);

    bool installDataFile(
        const std::string& path,
        const std::string& xmlFile,
        const std::string& destination);

    bool verifyXML(const std::string file);

private:
    OperationBuilder();
    virtual ~OperationBuilder();

    std::string makeIncludeString(const std::vector<std::string>& paths);

    /// Static unique instance.
    static OperationBuilder* instance_;
};

#endif
