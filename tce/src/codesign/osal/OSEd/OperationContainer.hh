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
 * @file OperationContainer.hh
 *
 * Declaration of OperationContainer class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_CONTAINER_HH
#define TTA_OPERATION_CONTAINER_HH

#include <string>

#include "PluginTools.hh"
#include "OperationContext.hh"
#include "BaseType.hh"

class Operation;
class OperationIndex;
class OperationSerializer;
class OperationModule;
class OperationBehavior;
class IdealSRAM;

/**
 * Singleton class that holds necessary information for viewing operation
 * properties, modifying operation behavior, and simulating operation
 * behavior.
 *
 * Include also means to view and modify the contents of the memory.
 */
class OperationContainer {
public:
    static OperationIndex& operationIndex();
    static OperationSerializer& operationSerializer();
    static OperationContext& operationContext();
    static Memory& memory();

    static void destroy();
	
    static OperationModule& module(
        const std::string& path, 
        const std::string& mod);

    static Operation* operation(
        const std::string& path,
        const std::string& module,
        const std::string& oper);

    static bool operationExists(const std::string& name);

    static bool isEffective(OperationModule& module, const std::string& name);
    
    static void freeBehavior(Operation& op, OperationModule& module);
    
    static Word memoryStart();
    static Word memoryEnd();
     
    virtual ~OperationContainer();

private:
    OperationContainer();
    /// Copying not allowed.
    OperationContainer(const OperationContainer&);
    /// Assignment not allowed.
    OperationContainer& operator=(const OperationContainer&);

    /// Creation function name for operation behavior.
    static const std::string CREATE_FUNCTION;
    /// Operation behavior deletion function name.
    static const std::string DELETE_FUNCTION;

    /// Starting point of the memory.
    static const Word MEMORY_START;
    /// End point of the memory.
    static const Word MEMORY_END;
    /// MAU size of the memory.
    static const Word MAUSIZE;

    /// Contains information of operations.
    static OperationIndex* index_;
    /// Static instance of operation serializer.
    static OperationSerializer* serializer_;
    /// Operation context used through the execution of the program.
    static OperationContext context_;
    /// Program counter.
    static InstructionAddress programCounter_;
    /// Return address.
    static SimValue returnAddress_;
    /// Sys call handler.
    static SimValue sysCallHandler_;
    /// Sys call number.
    static SimValue sysCallNumber_;
    /// Memory used througout the execution of the program.
    static IdealSRAM* memory_;
    /// PluginTools used by OperationContainer;
    static PluginTools tools_;
};

#endif
