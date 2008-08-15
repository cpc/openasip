/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file OperationContainer.cc
 *
 * Definition of OperationContainer class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <vector>

#include "OperationContainer.hh"
#include "OperationIndex.hh"
#include "OperationSerializer.hh"
#include "Operation.hh"
#include "OperationIndex.hh"
#include "OperationBehaviorLoader.hh"
#include "OperationBehaviorProxy.hh"
#include "Application.hh"
#include "PluginTools.hh"
#include "StringTools.hh"
#include "Environment.hh"
#include "OperationModule.hh"
#include "OperationBehavior.hh"
#include "IdealSRAM.hh"
#include "SimValue.hh"
#include "TCEString.hh"

using std::string;
using std::vector;

const string OperationContainer::CREATE_FUNCTION = "createOpBehavior_";
const string OperationContainer::DELETE_FUNCTION = "deleteOpBehavior_";
const Word OperationContainer::MEMORY_START = 0;
const Word OperationContainer::MEMORY_END = 65535;
const Word OperationContainer::MAUSIZE = 8;

OperationIndex* OperationContainer::index_ = NULL;
OperationSerializer* OperationContainer::serializer_ = NULL;
PluginTools OperationContainer::tools_;
InstructionAddress OperationContainer::programCounter_;
SimValue OperationContainer::returnAddress_;

IdealSRAM* OperationContainer::memory_ = 
    new IdealSRAM(MEMORY_START, MEMORY_END, MAUSIZE);

OperationContext OperationContainer::context_(
    memory_, programCounter_, returnAddress_);

/**
 * Constructor.
 */
OperationContainer::OperationContainer() {
}

/**
 * Destructor.
 */
OperationContainer::~OperationContainer() {
}

/**
 * Returns the instance of OperationIndex.
 *
 * @return The instance of OperationIndex.
 */
OperationIndex&
OperationContainer::operationIndex() {
    if (index_ == NULL) {
        index_ = new OperationIndex();
        vector<string> paths = Environment::osalPaths();
        for (size_t i = 0; i < paths.size(); i++) {
            index_->addPath(paths[i]);
        }
    }
    return *index_;
}

/**
 * Returns the instance of OperationSerializer.
 *
 * @return The instance of OperationSerializer.
 */
OperationSerializer&
OperationContainer::operationSerializer() {
    if (serializer_ == NULL) {
        serializer_ = new OperationSerializer();
    }
    return *serializer_;
}

/**
 * Returns the instance of OperationContext.
 *
 * @return Instance of OperationContext.
 */
OperationContext&
OperationContainer::operationContext() {
    return context_;
}

/**
 * Returns the memory model instance.
 *
 * @return Memory model instance.
 */
Memory&
OperationContainer::memory() {
    return *memory_;
}

/**
 * Returns a certain module in a certain path.
 *
 * If module is not found, a NullOperationModule is returned.
 *
 * @param path The name of the path.
 * @param mod The name of the module.
 * @return The module or NullOperationModule.
 */
OperationModule&
OperationContainer::module(const std::string& path, const std::string& mod) {
    OperationIndex& index = operationIndex();
    try {
        for (int i = 0; i < index.moduleCount(path); i++) {
            if (index.module(i, path).name() == mod) {
                return index.module(i, path);
            }
        }
    } catch (const Exception& e) {
        return NullOperationModule::instance();
    }
    return NullOperationModule::instance();
}

/**
 * Returns a certain operation in a certain module and a path.
 *
 * If operation is not found, NULL is returned.
 *
 * @param path The name of the path.
 * @param mod The name of the module.
 * @param oper The name of the operation.
 * @return The operation.
 */
Operation*
OperationContainer::operation(
    const std::string& path,
    const std::string& mod,
    const std::string& oper) {

    OperationModule& opModule = module(path, mod);
    assert(&opModule != &NullOperationModule::instance());

    OperationSerializer& serializer = operationSerializer();
    serializer.setSourceFile(opModule.propertiesModule());
    try {
        ObjectState* root = serializer.readState();
        for (int i = 0; i < root->childCount(); i++) {
            if (root->child(i)->stringAttribute("name") == oper) {

                OperationIndex* index = new OperationIndex;
                index->addPath(path);

                Operation* op = 
                    new Operation(oper, NullOperationBehavior::instance());
                op->loadState(root->child(i));

                OperationBehaviorLoader* behaviorLoader = new OperationBehaviorLoader(*index);
                OperationBehaviorProxy* behaviorProxy = new OperationBehaviorProxy(*op, *behaviorLoader);

                op->setBehavior(*behaviorProxy);

                delete root;
                return op;
            }
        }
    } catch (const Exception& e) {
        return NULL;
    }
    return NULL;
}

/**
 * Returns true if operation exists.
 *
 * @param name The name of the operation.
 * @return True if operation exists, false otherwise.
 */
bool
OperationContainer::operationExists(const std::string& name) {
    
    OperationIndex& index = operationIndex();

    for (int i = 0; i < index.moduleCount(); i++) {
        OperationModule& mod = index.module(i);
        for (int j = 0; j < index.operationCount(mod); j++) {
            if (index.operationName(j, mod) == name) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Returns true if operation is effective.
 *
 * Effective means that is is found first in list of search paths.
 *
 * @param module The module in which operation is defined.
 * @param name The name of the operation.
 * @return True if operation is effective.
 */
bool
OperationContainer::isEffective(
    OperationModule& module, 
    const std::string& name) {
    
    try {
        OperationIndex& index = operationIndex();
        for (int i = 0; i < index.pathCount(); i++) {
            string path = index.path(i);
            for (int j = 0; j < index.moduleCount(path); j++) {
                OperationModule& mod = index.module(j, path);
                for (int k = 0; k < index.operationCount(mod); k++) {
                    string opName = index.operationName(k, mod);
                    if (opName == name) {
                        if (&module == &mod) {
                            return true;
                        } else {
                            return false;
                        }
                    }
                }
            }
        }
    } catch (const Exception& e) {
        return false;
    }
    // never should come here
    assert(false);
    return false;
}

/**
 * Frees the behavior of the operation.
 *
 * @param op Operation of which behavior is freed.
 * @param module Module in which operation belongs to.
 */
void
OperationContainer::freeBehavior(
    Operation& op, 
    OperationModule& module) {

    try {
        string mod = module.behaviorModule();
        string func_name = DELETE_FUNCTION + 
            StringTools::stringToUpper(op.name());
        void (*function)(OperationBehavior*);
        tools_.importSymbol(func_name, function, mod);
        function(&op.behavior());
    } catch (const Exception& d) {
        return;
    }
}

/**
 * This function is called to clean up the static objects.
 *
 * This function should be called only when application is closed.
 */
void
OperationContainer::destroy() {
    
    if (index_ != NULL) {
        delete index_;
        index_ = NULL;
    }

    if (serializer_ != NULL) {
        delete serializer_;
        serializer_ = NULL;
    }

    if (memory_ != NULL) {
        delete memory_;
        memory_ = NULL;
    }
}

/**
 * Returns the start point of the memory.
 *
 * @return Memory start point.
 */
Word
OperationContainer::memoryStart() {
    return MEMORY_START;
}

/**
 * Returns the end point of the memory.
 *
 * @return Memory end point.
 */
Word
OperationContainer::memoryEnd() {
    return MEMORY_END;
}
