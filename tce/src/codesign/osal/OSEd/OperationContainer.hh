/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file OperationContainer.hh
 *
 * Declaration of OperationContainer class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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
