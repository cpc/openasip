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
 * @file ProGeContext.hh
 *
 * Declaration of ProGeContext class.
 *
 * Created on: 9.6.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef PROGECONTEXT_HH
#define PROGECONTEXT_HH

#include <string>
#include <memory>

#include "ProGeTypes.hh"

#include "Machine.hh"
#include "MachineImplementation.hh"

namespace TTAMachine {
    class Machine;
}

namespace IDF {
    class MachineImplementation;
}

namespace ProGe {

class GlobalPackage;

/*
 * Class that bundles all information that are needed to processor generation.
 */
class ProGeContext {
public:
    ProGeContext() = delete;
    ProGeContext(
        const TTAMachine::Machine& adf,
        const IDF::MachineImplementation& idf,
        const std::string& progeOutputDirectory,
        const std::string& sharedOutputDirectory,
        const std::string& coreEntityName,
        HDL targetHDL,
        int imemWidthInMAUs);

    virtual ~ProGeContext();

    const TTAMachine::Machine& adf() const;
    const IDF::MachineImplementation& idf() const;
    const std::string& outputDirectory() const;
    const std::string& sharedDeirectory() const;
    const std::string& coreEntityName() const;
    HDL targetHDL() const;

    const GlobalPackage& globalPackage() const;


private:

    /// The ADF.
    const TTAMachine::Machine& adf_;
    /// The IDF.
    const IDF::MachineImplementation& idf_;
    /// The target base directory under where generated files are placed.
    const std::string& progeOutputDirectory_;
    /// The target base directory for files that are shared across multiple
    /// TTAs.
    const std::string& sharedOutputDirectory_;
    /// The entity name of the cores.
    const std::string entityName_;
    /// The target language of generated files.
    HDL hdl_;
    // The instruction fetch block width in MAUs.
    //int imemWidthInMAUs_;
    /// The package that defines (multi)core wide constants.
    std::unique_ptr<GlobalPackage> globalPackage_;

};

} /* namespace ProGe */

#endif /* PROGECONTEXT_HH */
