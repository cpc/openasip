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
 * @file IPXactFileSerializer.cc
 *
 * Implementation of IPXactFileSerializer class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include "IPXactSerializer.hh"
#include "IPXactModel.hh"

const std::string IPXactSerializer::SPIRIT_NS_URI = 
    "http://www.spiritconsortium.org/XMLSchema/SPIRIT/1.2";

IPXactSerializer::IPXactSerializer(): XMLSerializer() {
    
    setUseSchema(false);
}


IPXactSerializer::~IPXactSerializer() {
}


void
IPXactSerializer::writeState(const ObjectState* ipXactState) 
    throw (SerializerException){
    
    XMLSerializer::setXMLNamespace(SPIRIT_NS_URI);
    XMLSerializer::writeState(ipXactState);
}

void
IPXactSerializer::writeIPXactModel(const IPXactModel& model) {

    ObjectState* omState = model.saveState();
    writeState(omState);
    delete omState;
}


ObjectState*
IPXactSerializer::readState() throw (SerializerException) {
    
    return NULL;
}
