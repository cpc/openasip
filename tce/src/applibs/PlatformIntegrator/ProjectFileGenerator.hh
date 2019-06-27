/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file ProjectFileGenerator.hh
 *
 * Declaration of ProjectFileGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_PROJECT_FILE_GENERATOR_HH
#define TTA_PROJECT_FILE_GENERATOR_HH

#include <vector>
#include "TCEString.hh"
#include "PlatformIntegratorTypes.hh"

class PlatformIntegrator;

class ProjectFileGenerator {
public:
    ProjectFileGenerator(TCEString coreEntity,
                         const PlatformIntegrator* integrator);
    virtual ~ProjectFileGenerator();

    virtual void writeProjectFiles() = 0;

    void addHdlFile(const TCEString& file);

    void addHdlFiles(const std::vector<TCEString>& files);

    void addMemInitFile(const TCEString& memInit);

    void addSignalMapping(const PlatInt::SignalMapping& mapping);

protected:

    const std::vector<TCEString>& hdlFileList() const;

    const std::vector<TCEString>& memInitFileList() const;
    
    const PlatformIntegrator* integrator() const;

    TCEString coreEntity() const { return coreEntity_; }
    TCEString toplevelEntity() const {
        return coreEntity_ + "_toplevel";
    }

    int signalMappingCount() const;

    const PlatInt::SignalMapping* signalMapping(int index) const;

    TCEString extractFUName(
        const TCEString& port,
        const TCEString& delimiter) const;

private:
    /// the HDL entity of the non-integrated TTA core
    TCEString coreEntity_;
    const PlatformIntegrator* integrator_;
    
    std::vector<TCEString> hdlFiles_;
    std::vector<TCEString> memInitFiles_;
    PlatInt::SignalMappingList signalMap_;
};

#endif
