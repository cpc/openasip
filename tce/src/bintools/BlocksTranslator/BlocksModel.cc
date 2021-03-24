/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file BlocksModel.cc
 *
 * Implementation of the Blocks utility class which contains a Blocks model.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#include "BlocksModel.hh"

#include <exception>
#include <iostream>

using namespace std;

/**
 * Load and parse the Blocks 'architecture.xml'.
 */
void
BlocksModel::LoadModelFromXml() {
    // TODO(mm): add check for configuration bits
    if (!VerifyXmlStructure()) return;

    ObjectState* configs = mdfState_->childByName("configuration");
    ObjectState* fuState = configs->childByName("functionalunits");

    for (int i = 0; i < fuState->childCount(); i++) {
        ObjectState* fu = fuState->child(i);
        FunctionalUnit unit = {};
        unit.usesOut0 = false;
        unit.usesOut1 = false;
        string unitTypeString = fu->stringAttribute("type");
        unit.type = stringToEnum.at(unitTypeString);
        unit.name = fu->stringAttribute("name");

        // FU ports
        for (int srcnum = 0; srcnum < fu->childCount(); srcnum++) {
            ObjectState* srcPort = fu->child(srcnum);
            unit.src.push_back(srcPort->stringAttribute("source"));
        }

        mFunctionalUnitList.push_back(unit);  // Add functional unit to list
    }
}

/**
 * Verify the structure of the 'architecture.xml' file.
 */
bool
BlocksModel::VerifyXmlStructure() {
    return mdfState_->hasChild("configuration") &&
           mdfState_->hasChild("Core");
}
