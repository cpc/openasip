/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file LLVMIRGen.hh
 *
 * Declaration of LLVMIRGen class.
 *
 * Created on: 17.3.2015
 * @author: Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef LLVMIRGEN_HH
#define LLVMIRGEN_HH

#include <string>
#include <iostream>

#include "Operation.hh"
#include "TCEString.hh"
#include "TCETargetDataLayout.hh"

class Operand;

/*
 * Helper class to write LLVM IR assembly code sections.
 */
class LLVMIRTools {
public:
    LLVMIRTools();
    virtual ~LLVMIRTools();

    class Type {
    public:
        typedef std::vector<Type> TypeList;
        Type();
        Type(stringCRef type, int alignment);
        virtual ~Type();
        std::string toString() const;
        void addSubType(const Type& other);
        int getAlignment() const;
        bool isStructure() const;
        bool isHalfFloat() const;
        const TypeList& subTypes() const;

    private:
        std::string type_;
        int alignment_;
        TypeList subTypes_;
    };

    std::string targetInfo(bool littleEndian = false) const;
    std::string mainBody(
        stringCRef mainCode) const;
    std::string assignment(
        stringCRef identifierField,
        stringCRef rest) const;
    std::string inlineAssembly(
        stringCRef operationField,
        stringCRef returnTypeListField,
        stringCRef asmArgumentsField,
        stringCRef metadata = "") const;
    std::string store(
        stringCRef toStoredField,
        stringCRef destinationField,
        stringCRef aligment,
        bool setVolatile = false) const;
    std::string store(
        const Type& nonAggregateType,
        stringCRef toStoredIdentifier,
        stringCRef destinationIdentifier,
        bool setVolatile = false) const;
    std::string extractValue(
        stringCRef destIdentifier,
        stringCRef srcIdentifier,
        const Type& structureType,
        unsigned int fieldSelect) const;

    std::string inlineAssemblyArguments(
        const Operation& operation,
        const Operation::InputOperandVector& inputValues) const;
    Type outputTypeListOf(
        const Operation& operation) const;
    std::string constraints(
        const Operation& operation) const;
    std::string arguments(
        const Operation& operation,
        const Operation::InputOperandVector& inputValues) const;

    std::string newGlobalDecl(
        stringCRef typeAndInitialValue,
        stringCRef aligmentValue);
    std::string newGlobalDecl(
        const Operand& operandInfo,
        const SimValue& value);
    std::string lastGlobalIdentifier() const;

    std::string newIdentifier();
    std::string alignment(
        const Operand& operandInfo) const;
    int alignmentValue(
        const Operand& operandInfo) const;
    Type typeOf(
        const Operand& operandInfo) const;
    std::string pointerTypeOf(
        const Operand& operandInfo) const;
    std::string pointerOfTo(
        const Operand& operandInfo,
        stringCRef identifier) const;

    std::string constant(
        const Operand& operandInfo,
        const SimValue& value) const;
    std::string constant(
        SIntWord value) const;
    std::string constant(
        UIntWord value) const;
    std::string constant(
        HalfFloatWord value) const;
    std::string constant(
        FloatWord value) const;
    std::string constant(
        DoubleWord value) const;
    void resetIdentifiers();
private:

    Type singleTypeOf(
        const Operand& operandInfo) const;
    std::string bothNonEmpty(
        stringCRef str1,
        stringCRef str2) const;

    unsigned int lastReturnedTempIdentifier_;
    unsigned int lastReturnedGlobalIdentifier_;

};

#endif /* LLVMIRGEN_HH */
