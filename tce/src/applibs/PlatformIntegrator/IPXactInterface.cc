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
 * @file IPXactInterface.cc
 *
 * Implementation of IPXactInterface class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include "IPXactInterface.hh"
using std::pair;

IPXactInterface::IPXactInterface():
    instanceName_(""), busMode_(IPXactModel::INVALID), busType_("","","",""),
    busAbstractionType_("","","",""), interfaceMap_() {
}

IPXactInterface::~IPXactInterface() {
}

TCEString
IPXactInterface::instanceName() const {
    return instanceName_;
}

IPXact::Vlnv
IPXactInterface::busType() const {

    return busType_;
}

IPXact::Vlnv
IPXactInterface::busAbstractionType() const {

    return busAbstractionType_;
}

IPXactModel::BusMode
IPXactInterface::busMode() const {

    assert(busMode_ != IPXactModel::INVALID && "Bus mode not set!");
    return busMode_;
}

const SignalMappingList&
IPXactInterface::interfaceMapping() const {

    return interfaceMap_;
}

void
IPXactInterface::addSignalMapping(
    const TCEString& actualSignal,
    const TCEString& busSignal) {

    interfaceMap_.push_back(
        pair<TCEString,TCEString>(actualSignal, busSignal));
}

void
IPXactInterface::setInstanceName(const TCEString& name) {
    
    instanceName_ = name;
}

void
IPXactInterface::setBusMode(IPXactModel::BusMode mode) {
    
    busMode_ = mode;
}

void 
IPXactInterface::setBusType(
    const TCEString& vendor,
    const TCEString& library,
    const TCEString& name,
    const TCEString& version) {

    busType_.vendor = vendor;
    busType_.library = library;
    busType_.name = name;
    busType_.version = version;
}

void
IPXactInterface::setBusAbsType(
    const TCEString& vendor,
    const TCEString& library,
    const TCEString& name,
    const TCEString& version) {

    busAbstractionType_.vendor = vendor;
    busAbstractionType_.library = library;
    busAbstractionType_.name = name;
    busAbstractionType_.version = version;
}
    
