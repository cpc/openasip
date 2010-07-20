/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "PlatformIntegrator.hh"

typedef std::pair<std::string, std::string> SignalMapping;

class ProjectFileGenerator {
public:
    ProjectFileGenerator(std::string toplevelEntity,
                         PlatformIntegrator* integrator);
    virtual ~ProjectFileGenerator();

    virtual void writeProjectFiles() = 0;

    void addHdlFile(const std::string& file);

    void addSignalMapping(const SignalMapping& mapping);

protected:

    const std::vector<std::string>& hdlFileList() const;
    
    const PlatformIntegrator* integrator() const;

    std::string toplevelEntity() const;

    int signalMappingCount() const;

    const SignalMapping* signalMapping(int index) const;

private:
    std::string toplevelEntity_;
    const PlatformIntegrator* integrator_;
    
    std::vector<std::string> hdlFiles_;
    std::vector<SignalMapping> signalMap_;
};

#endif
