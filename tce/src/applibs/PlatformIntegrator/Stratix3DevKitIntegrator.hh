/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file Stratix3DevKitIntegrator.hh
 *
 * Declaration of Stratix3DevKitIntegrator class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_STRATIX3_DEV_KIT_INTEGRATOR_HH
#define TTA_STRATIX3_DEV_KIT_INTEGRATOR_HH

#include <iostream>
#include <vector>
#include <map>
#include "TCEString.hh"
#include "AlteraIntegrator.hh"
#include "QuartusProjectGenerator.hh"
#include "PlatformIntegratorTypes.hh"

class Stratix3DevKitIntegrator : public AlteraIntegrator {
public:

    Stratix3DevKitIntegrator();

    Stratix3DevKitIntegrator(
        const TTAMachine::Machine* machine,
        const IDF::MachineImplementation* idf,
        ProGe::HDL hdl,
        TCEString progeOutputDir,
        TCEString coreEntityName,
        TCEString outputDir,
        TCEString programName,
        int targetClockFreq,
        std::ostream& warningStream,
        std::ostream& errorStream,
        const MemInfo& imem,
        MemType dmemType);

    virtual ~Stratix3DevKitIntegrator();

    virtual void integrateProcessor(const ProGe::NetlistBlock* ttaCore);
    
    virtual TCEString deviceFamily() const;

    virtual void setDeviceFamily(TCEString devFamily);

    virtual TCEString deviceName() const;
    
    virtual TCEString devicePackage() const;

    virtual TCEString deviceSpeedClass() const;

    virtual int targetClockFrequency() const;

    virtual void printInfo(std::ostream& stream) const;

protected:

    virtual TCEString pinTag() const;

    virtual bool chopTaggedSignals() const;

    virtual ProjectFileGenerator* projectFileGenerator() const;

private:

    void generatePinMap();

    void mapToplevelPorts();

    void addSignalMapping(const TCEString& signal);

    QuartusProjectGenerator* quartusGen_;

    PlatInt::PinMap stratix3Pins_;

    static const TCEString DEVICE_FAMILY_;

    static const TCEString DEVICE_NAME_;

    static const TCEString DEVICE_PACKAGE_;

    static const TCEString DEVICE_SPEED_CLASS_;

    static const TCEString PIN_TAG_;

    static const int DEFAULT_FREQ_;

};

#endif
