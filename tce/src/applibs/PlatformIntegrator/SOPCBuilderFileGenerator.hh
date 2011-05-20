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
 * @file SOPCBuilderFileGenerator.hh
 *
 * Declaration of SOPCBuilderFileGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOPC_BUILDER_FILE_GENERATOR_HH
#define TTA_SOPC_BUILDER_FILE_GENERATOR_HH

#include <iostream>
#include <map>
#include "ProjectFileGenerator.hh"
#include "HDBTypes.hh"
#include "SOPCInterface.hh"
#include "NetlistPort.hh"

class AvalonMMMasterInterface;

class SOPCBuilderFileGenerator : public ProjectFileGenerator {
public:
    
    SOPCBuilderFileGenerator(TCEString toplevelEntity,
                             const PlatformIntegrator* integrator);

    virtual ~SOPCBuilderFileGenerator();

    virtual void writeProjectFiles();
    
private:

    int countAvalonMMMasters() const;

    void createInterfaces();

    bool handleAvalonSignal(const ProGe::NetlistPort& port);

    void exportSignal(const ProGe::NetlistPort& port);

    void writeModuleProperties(std::ostream& stream);

    void writeGenerics(std::ostream& stream);

    void writeFileList(std::ostream& stream);

    void writeInterfaces(std::ostream& stream) const;

    AvalonMMMasterInterface* getMaster(const TCEString& fuName);

    SOPCInterface* clock_;
    SOPCInterface* export_;
    std::map<TCEString, AvalonMMMasterInterface*> masters_;


    static const TCEString HDB_AVALON_PREFIX;
    static const TCEString SOPC_COMPONENT_FILE_TYPE;

    static const TCEString TTA_CLOCK_NAME;
    static const TCEString TTA_RESET_NAME;
    static const TCEString SOPC_CLOCK_NAME;
    static const TCEString SOPC_RESET_NAME;

    static const TCEString SOPC_DEFAULT_GROUP;
    static const TCEString SOPC_DEFAULT_VHDL_LIBS;
    static const TCEString SOPC_SET_MODULE_PROPERTY;
    static const TCEString SOPC_ADD_FILE;

    static const TCEString PI_DEVICE_FAMILY_GENERIC;

};

#endif
