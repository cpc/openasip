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
 * @file Stratix2DSPBoardIntegrator.hh
 *
 * Declaration of Stratix2DSPBoardIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_STRATIX2_DSP_BOARD_INTEGRATOR_HH
#define TTA_STRATIX2_DSP_BOARD_INTEGRATOR_HH

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include "PlatformIntegrator.hh"
#include "QuartusProjectGenerator.hh"

class Stratix2DSPBoardIntegrator : public PlatformIntegrator {
public:

    Stratix2DSPBoardIntegrator();

    Stratix2DSPBoardIntegrator(
        std::string progeOutputDir,
        std::string entityName,
        std::string outputDir,
        std::string programName,
        int targetClockFreq,
        std::ostream& warningStream,
        std::ostream& errorStream);
    
    virtual ~Stratix2DSPBoardIntegrator();

    virtual void integrateProcessor(MemInfo& imem, MemInfo& dmem);

    virtual std::string deviceFamily() const;

    virtual std::string deviceName() const;
    
    virtual std::string devicePackage() const;

    virtual std::string deviceSpeedClass() const;

    virtual int targetClockFrequency() const;

    virtual void printInfo(std::ostream& stream) const;

private:

    bool createSignals(const MemInfo& imem, const MemInfo& dmem);

    bool createMemories(const MemInfo& imem, const MemInfo& dmem);

    void writeProjectFiles();

    void createPinMap();

    void finishNewToplevel();

    void addSignalMapping(const std::string& signal);

    QuartusProjectGenerator* quartusGen_; 

    typedef std::map<std::string, std::vector<std::string>*> PinMap;
    PinMap stratixPins_;

    static const std::string DEVICE_FAMILY_;

    static const std::string DEVICE_NAME_;

    static const std::string DEVICE_PACKAGE_;

    static const std::string DEVICE_SPEED_CLASS_;

    static const std::string PIN_TAG_;

    static const int DEFAULT_FREQ_;
};

#endif
