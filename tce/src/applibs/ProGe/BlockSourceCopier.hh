/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file BlockSourceCopier.hh
 *
 * Declaration of BlockSourceCopier class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @note rating: red
 */

#ifndef TTA_BLOCK_SOURCE_COPIER_HH
#define TTA_BLOCK_SOURCE_COPIER_HH

#include <set>

#include "TCEString.hh"
#include "RFImplementationLocation.hh"

namespace IDF {
    class MachineImplementation;
}

namespace HDB {
    class HWBlockImplementation;
}

namespace ProGe {

/**
 * Copies the block definition files used in the processor implementation to
 * the output directory of ProGe.
 */
class BlockSourceCopier {
public:
    BlockSourceCopier(
        const IDF::MachineImplementation& implementation,
        TCEString entityStr);
    virtual ~BlockSourceCopier();

    void copyShared(const std::string& dstDirectory)
        throw (IOException);

    void copyProcessorSpecific(const std::string& dstDirectory)
        throw (IOException);

private:
    void copyBaseRFFiles(
        const IDF::RFImplementationLocation& implementation,
        const std::string& dstDirectory)
        throw (IOException);
    void copyFiles(
        const HDB::HWBlockImplementation& implementation,
        const std::string& hdbFile,
        const std::string& dstDirectory)
        throw (UnreachableStream, FileNotFound);

    void copyFromTemplate(
        const std::string& templateFile,
        const std::string& dstDirectory)
        throw (UnreachableStream, FileNotFound);

    void setCopied(const std::string& file);
    bool isCopied(const std::string& file) const;

    /// The IDF object model.
    const IDF::MachineImplementation& implementation_;
    /// Copied files.
    std::set<std::string> copiedFiles_;
    TCEString entityStr_;
};
}

#endif
