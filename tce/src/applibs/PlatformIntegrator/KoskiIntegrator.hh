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
 * @file KoskiIntegrator.hh
 *
 * Declaration of KoskiIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_KOSKI_INTEGRATOR_HH
#define TTA_KOSKI_INTEGRATOR_HH

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include "PlatformIntegrator.hh"


class KoskiIntegrator : public PlatformIntegrator {
public:
    KoskiIntegrator();

    KoskiIntegrator(
        ProGe::HDL hdl,
        std::string progeOutputDir,
        std::string entityName,
        std::string outputDir,
        std::string programName,
        int targetClockFreq,
        std::ostream& warningStream,
        std::ostream& errorStream);

    virtual ~KoskiIntegrator();

    virtual void integrateProcessor(
        const ProGe::NetlistBlock* ttaCore,
        MemInfo& imem,
        MemInfo& dmem);

    virtual std::string deviceFamily() const;

    virtual std::string deviceName() const;
    
    virtual std::string devicePackage() const;

    virtual std::string deviceSpeedClass() const;

    virtual int targetClockFrequency() const;

    virtual void printInfo(std::ostream& stream) const;

protected:

    virtual std::string pinTag() const;
    
    virtual bool isDataMemorySignal(const std::string& signalName) const;

    virtual ProjectFileGenerator* projectFileGenerator() const;

    virtual MemoryGenerator* imemInstance(const MemInfo& imem);

    virtual MemoryGenerator* dmemInstance(const MemInfo& dmem);

private:

    ProjectFileGenerator* ipXactGen_;

    static const std::string PIN_TAG_;
};


#endif
