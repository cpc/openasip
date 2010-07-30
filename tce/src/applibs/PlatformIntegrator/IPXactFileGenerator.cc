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
using ProGe::NetlistBlock;
using ProGe::NetlistPort;
using std::pair;
using std::string;

const std::string DEF_VENDOR = "TCE";
const std::string DEF_LIBRARY = "Koski";
const int DEF_VERSION = 1;


IPXactFileGenerator::IPXactFileGenerator(
    std::string toplevelEntity,
    const PlatformIntegrator* integrator):
    ProjectFileGenerator(toplevelEntity, integrator),
    ipXactWriter_(new IPXactSerializer()), busInterfaces_() {

    SignalMappingList* hibi = new SignalMappingList();
    hibi->push_back(pair<string,string>("hibi_comm_out", "COMM_FROM_IP"));
    hibi->push_back(pair<string,string>("hibi_data_out", "DATA_FROM_IP"));
    hibi->push_back(pair<string,string>("hibi_av_out", "AV_FROM_IP"));
    hibi->push_back(pair<string,string>("hibi_we_out", "WE_FROM_IP"));
    hibi->push_back(pair<string,string>("hibi_re_out", "RE_FROM_IP"));
    hibi->push_back(pair<string,string>("hibi_comm_in", "COMM_TO_IP"));
    hibi->push_back(pair<string,string>("hibi_data_in", "DATA_TO_IP"));
    hibi->push_back(pair<string,string>("hibi_av_in", "AV_TO_IP"));
    hibi->push_back(pair<string,string>("hibi_full_in", "FULL_TO_IP"));
    hibi->push_back(pair<string,string>("hibi_empty_in", "EMPTY_TO_IP"));
    busInterfaces_[IPXactModel::HIBI] = hibi;
}

IPXactFileGenerator::~IPXactFileGenerator() {
    
    BusMapping::iterator iter = busInterfaces_.begin();
    while (iter != busInterfaces_.end()) {
        delete iter->second;
        iter++;
    }
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
    addBusInterfaces(ip);

    ip->setHdlFiles(hdlFileList());
    for (unsigned int i = 0; i < memInitFileList().size(); i++) {
        ip->setFile(memInitFileList().at(i));
    }
    
    ipXactWriter_->setDestinationFile(outputFileName());
    ipXactWriter_->writeIPXactModel(*ip);
}

void
IPXactFileGenerator::addBusInterfaces(IPXactModel* model) {
    
    BusMapping::iterator iter = busInterfaces_.begin();
    while (iter != busInterfaces_.end()) {
        IPXactModel::IPXactBus bus = iter->first;
        const SignalMappingList& search = *iter->second;
        SignalMappingList mapping;
        if (searchInterface(search, mapping)) {
            model->addBusInterface(bus, mapping);
        }
        iter++;
    }
}

bool
IPXactFileGenerator::searchInterface(
    const SignalMappingList& search,
    SignalMappingList& found) const {;
    
    const ProGe::NetlistBlock& toplevel = integrator()->toplevelBlock();
    for (int i = 0; i < toplevel.portCount(); i++) {
        NetlistPort& port = toplevel.port(i);
        for (unsigned int j = 0; j < search.size(); j++) {
            if (port.name().find(search.at(j).first) != string::npos) {
                // add mapping from toplevel signal name to bus signal name
                found.push_back(
                    pair<string,string>(port.name(), search.at(j).second));
            }
        }
    }
    bool foundAll = false;
    if (search.size() == found.size()) {
        foundAll = true;
    }
    return foundAll;
}


std::string
IPXactFileGenerator::outputFileName() const {

    return "spirit_comp_def_" + toplevelEntity() + ".xml";
}
