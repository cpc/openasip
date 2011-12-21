/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file OperationPropertyLoader.cc
 *
 * Definition of OperationPropertyLoader class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#include <string>
#include <vector>

#include "OperationPropertyLoader.hh"
#include "SequenceTools.hh"
#include "OperationModule.hh"
#include "Operation.hh"
#include "TCEString.hh"
#include "ObjectState.hh"

using std::string;
using std::vector;

/**
 * Constructor.
 */
OperationPropertyLoader::OperationPropertyLoader() {
}

/**
 * Destructor.
 */
OperationPropertyLoader::~OperationPropertyLoader() {
    MapIter it = operations_.begin();
    while (it != operations_.end()) {
        SequenceTools::deleteAllItems((*it).second);
        it++;
    }
}

/**
 * Loads the properties of an operation.
 *
 * If operation properties has not yet been loaded, it is done first.
 *
 * @param operation Operation which properties are loaded and updated.
 * @param module Module where properties are loaded from.
 * @exception InstanceNotFound If reading of XML fails, if Operation fails to
 *                             load its state, or if properties are not found 
 *                             at all.
 */
void
OperationPropertyLoader::loadOperationProperties(
    Operation& operation, 
    const OperationModule& module)
    throw (InstanceNotFound) {

    MapIter it = operations_.find(module.propertiesModule());
    if (it == operations_.end()) {
        // properties are not yet read, let's do it first
        loadModule(module);
        it = operations_.find(module.propertiesModule());
    }

    const vector<ObjectState*> ops = (*it).second;
    bool operationFound = false;
    
    for (unsigned int i = 0; i < ops.size(); i++) {
        if (ops[i]->stringAttribute(Operation::OPRN_NAME) == 
            operation.name()) {
            
            operationFound = true;
            try {
                operation.loadState(ops[i]);
            } catch (const ObjectStateLoadingException& o) {
                string msg = "Problems when loading the state of the Object: "
                    + o.errorMessage();
                throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
            }
            break;
        }
    }

    if (!operationFound) {
        string msg = string("Properties for operation ") + operation.name() + 
            " not found";
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Loads the module.
 *
 * Saves its operations as an ObjectState objects.
 * 
 * @param module The OperationModule.
 * @exception InstanceNotFound If loading the module fails.
 */
void
OperationPropertyLoader::loadModule(const OperationModule& module) 
    throw (InstanceNotFound) {
    
    serializer_.setSourceFile(module.propertiesModule());
    ObjectState* root = NULL;
    try {
        root = serializer_.readState();
    } catch (const SerializerException& s) {
        string msg = "Problems when reading the XML file: " + s.errorMessage();
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    }
    vector<ObjectState*> ops;
    for (int i = 0; i < root->childCount(); i++) {
        ObjectState* child = new ObjectState(*root->child(i));
        ops.push_back(child);
    }
    delete root;
    operations_.insert(ValueType(module.propertiesModule(), ops));
}
