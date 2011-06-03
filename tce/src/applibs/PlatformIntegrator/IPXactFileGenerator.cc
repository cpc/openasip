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
 * @file IPXactFileGenerator.hh
 *
 * Implementation of IPXactFileGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include <string>
#include <vector>
#include "PlatformIntegrator.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "IPXactFileGenerator.hh"
#include "IPXactSerializer.hh"
#include "IPXactModel.hh"
#include "HDLPort.hh"
#include "IPXactInterface.hh"
#include "IPXactHibiInterface.hh"
#include "IPXactClkInterface.hh"
#include "IPXactResetInterface.hh"
#include "IPXactAddressSpace.hh"
#include "AddressSpace.hh"

#include "Machine.hh"
using ProGe::NetlistBlock;
using ProGe::NetlistPort;
using std::pair;

const TCEString DEF_VENDOR = "TCE";
const TCEString DEF_LIBRARY = "ip.hwp.tta";
const TCEString DEF_VERSION = "1.0";


IPXactFileGenerator::IPXactFileGenerator(
    TCEString toplevelEntity,
    const PlatformIntegrator* integrator):
    ProjectFileGenerator(toplevelEntity, integrator),
    ipXactWriter_(new IPXactSerializer()) {

}

IPXactFileGenerator::~IPXactFileGenerator() {
    
    delete ipXactWriter_;
}


void
IPXactFileGenerator::writeProjectFiles() {

    IPXactModel* ip = new IPXactModel();
    ip->setVLNV(DEF_VENDOR, DEF_LIBRARY, toplevelEntity(), DEF_VERSION);

    const ProGe::NetlistBlock& toplevel = integrator()->toplevelBlock();
    for (int i = 0; i < toplevel.portCount(); i++) {
        HDLPort port(toplevel.port(i));
        ip->addSignal(port);
    }

    for (int i = 0; i < toplevel.parameterCount(); i++) {
        ip->addParameter(toplevel.parameter(i));
    }

    addBusInterfaces(ip);

    addAddressSpaces(ip);

    ip->setHdlFiles(hdlFileList());
    for (unsigned int i = 0; i < memInitFileList().size(); i++) {
        ip->setFile(memInitFileList().at(i));
    }
    
    ipXactWriter_->setDestinationFile(outputFileName());
    ipXactWriter_->writeIPXactModel(*ip);
    delete ip;
}

void
IPXactFileGenerator::addBusInterfaces(IPXactModel* model) {

    std::vector<IPXactInterface*> interfaces;
    interfaces.push_back(new IPXactClkInterface());
    interfaces.push_back(new IPXactResetInterface());
    interfaces.push_back(new IPXactHibiInterface());
    

    const NetlistBlock& toplevel = integrator()->toplevelBlock();
    for (unsigned int i = 0; i < interfaces.size(); i++) {
        if (interfaces.at(i)->mapPortsToInterface(toplevel)) {
            // mapping was successful, add interface
            model->addBusInterface(interfaces.at(i));
        } else {
            delete interfaces.at(i);
            interfaces.at(i) = NULL;
        }
    }
}


void
IPXactFileGenerator::addAddressSpaces(IPXactModel* model) {
    
    IPXactAddressSpace* imemAs =
        new IPXactAddressSpace(integrator()->imemInfo());
    IPXactAddressSpace* dmemAs =
        new IPXactAddressSpace(integrator()->dmemInfo());
    model->addAddressSpace(imemAs);
    model->addAddressSpace(dmemAs);
}


TCEString
IPXactFileGenerator::outputFileName() const {

    return "spirit_comp_def_" + toplevelEntity() + ".xml";
}
