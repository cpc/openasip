
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
 * @file LLVMIRTools.cc
 *
 * Implementation of LLVMIRTools class.
 *
 * Created on: 17.3.2015
 * @author: Henry Linjam�ki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "LLVMIRTools.hh"

#include <sstream>
#include <algorithm>

#include "Operand.hh"
#include "Operation.hh"
#include "Conversion.hh"
#include "MathTools.hh"

LLVMIRTools::LLVMIRTools()
    : lastReturnedTempIdentifier_(0), lastReturnedGlobalIdentifier_(0) {
}

LLVMIRTools::~LLVMIRTools() {
}

LLVMIRTools::Type::Type()
    : type_(""), alignment_(0), subTypes_() {
}

LLVMIRTools::Type::Type(stringCRef type, int alignment)
    : type_(type), alignment_(alignment), subTypes_() {
}

LLVMIRTools::Type::~Type() {
}

std::string
LLVMIRTools::Type::toString() const {
    if (isStructure()) {
        std::string structureType("{ ");
        for (size_t i = 0; i < subTypes_.size(); i++) {
            structureType += TCEString::applyIf(i > 0, ", ") +
                subTypes_.at(i).toString();
        }
        structureType += " }";
        return structureType;
    } else if (type_.empty()) {
        return "void";
    } else {
        return type_;
    }
}

void
LLVMIRTools::Type::addSubType(const LLVMIRTools::Type& other) {
    assert(this != &other);
    subTypes_.push_back(other);
}

int
LLVMIRTools::Type::getAlignment() const {
    return alignment_;
}

bool
LLVMIRTools::Type::isStructure() const {
    return !subTypes_.empty(); // Even if there is only one subtype defined.
}

bool
LLVMIRTools::Type::isHalfFloat() const {
    return type_ == "half";
}

const LLVMIRTools::Type::TypeList&
LLVMIRTools::Type::subTypes() const {
    return subTypes_;
}

/**
 * Writes target information, which includes datalayout and triple information.
 */
std::string
LLVMIRTools::targetInfo(bool littleEndian) const {
    std::string targetInfoStr = "target datalayout = \"";
    targetInfoStr += TCEString::applyIf(littleEndian,"e", "E");
    if(littleEndian){
        targetInfoStr += "-p:32:32:32-i1:8:8-i8:8:32-i16:16:32-i32:32:32";
        targetInfoStr += "-i64:32:32-f32:32:32-f64:32:32-v64:64:64-v128:128:128";
        targetInfoStr += "-v256:256:256-v512:512:512-v1024:1024:1024";
        targetInfoStr += "-v2048:2048:2048-v4096:4096:4096-a0:0:32-n32";
    }
    else{
        targetInfoStr += "-p:32:32-i8:8:32-i16:16:32-i64:32-f64:32-v64:32-";
        targetInfoStr += "v128:32-v256:32-v512:32-v1024:32-a:0:32-n32";
    }
    targetInfoStr += "\"\n";
    targetInfoStr += std::string("target triple = \"")
        + TCEString::applyIf(littleEndian,"tcele-tut-llvm", "tce-tut-llvm")
        + "\"\n";

    return targetInfoStr;
}

/**
 * Wraps given LLVM code in main function.
 */
std::string
LLVMIRTools::mainBody(stringCRef mainCode) const {
    std::string mainStr;
    mainStr += std::string("@__dummy__ = internal global i32 0, align 4")
        + "\n\n"
        + "define void @_start() #0 {" + "\n"
        + "entry:\n"
        + "  tail call void asm sideeffect \".call_global_ctors\", \"\"() #1"
        + "\n"

        + mainCode + "\n"

        + "  tail call void asm sideeffect \".call_global_dtors\", \"\"() #1"
        + "\n"
        + "  tail call fastcc void @_exit() #2" + "\n"
        + "  unreachable" + "\n"
        + "}" + "\n" + "\n"
        + "; Function Attrs: noinline noreturn nounwind" + "\n"
        + "define internal fastcc void @_exit() #0 {" + "\n"
        + "entry:" + "\n"
        + "  br label %while.body" + "\n" + "\n"
        + "while.body: ; preds = %while.body, %entry" + "\n"
        + "  store volatile i32 0, ptr @__dummy__, align 4" + "\n"
        + "  br label %while.body" + "\n"
        + "}" + "\n" + "\n"
        + "attributes #0 = { noinline noreturn nounwind "
        + "\"less-precise-fpmad\"=\"false\" "
        + "\"no-frame-pointer-elim\"=\"true\" "
        + "\"no-frame-pointer-elim-non-leaf\" "
        + "\"no-infs-fp-math\"=\"false\" "
        + "\"no-nans-fp-math\"=\"false\" "
        + "\"stack-protector-buffer-size\"=\"8\" "
        + "\"unsafe-fp-math\"=\"false\" "
        + "\"use-soft-float\"=\"false\" }" + "\n"
        + "attributes #1 = { nounwind }" + "\n"
        + "attributes #2 = { nobuiltin noreturn }" + "\n";
    return mainStr;
}

/**
 * Composes assignment string "<identifierField> = <rest>";
 */
std::string
LLVMIRTools::assignment(
    stringCRef identifierField,
    stringCRef rest) const {

    return identifierField + " = " + rest;
}

/**
 * Composes string "tail call <returnTypes> asm <oper> <args> <metadata>"
 *
 * Fields maps for returnTypes to returnTypeListField, oper to operationField,
 * and args to asmArgumentsField.
 */
std::string
LLVMIRTools::inlineAssembly(
    stringCRef operationField,
    stringCRef returnTypeListField,
    stringCRef asmArgumentsField,
    stringCRef metadata) const  {

    return std::string("tail call ") +
        returnTypeListField + " asm " + "\"" +
        operationField + "\", " + asmArgumentsField +
        bothNonEmpty(" ", metadata);
}

/**
 * Composes string of "store <volatile> <srcField>, <dstField>, <align>"
 *
 */
std::string
LLVMIRTools::store(
    stringCRef toStoredField,
    stringCRef destinationField,
    stringCRef aligment,
    bool setVolatile) const {

    return std::string("store ")
         + TCEString::applyIf(setVolatile, "volatile ")
         + toStoredField + ", " + destinationField
         + TCEString::applyIf(!aligment.empty(), ", ") + aligment;
}

/**
 * Composes string of store instruction for given Type.
 */
std::string
LLVMIRTools::store(
    const LLVMIRTools::Type& nonAggregateType,
    stringCRef toStoredIdentifier,
    stringCRef destinationIdentifier,
    bool setVolatile) const {

    return std::string("store ")
        + TCEString::applyIf(setVolatile, "volatile ")
        + nonAggregateType.toString() + " " + toStoredIdentifier + ", "
        + "ptr " + destinationIdentifier + ", "
        + "align " + Conversion::toString(nonAggregateType.getAlignment());
}

/**
 * Composes extractvalue instruction string.
 */
std::string
LLVMIRTools::extractValue(
    stringCRef destIdentifier,
    stringCRef srcIdentifier,
    const Type& structureType,
    unsigned int fieldSelect) const {

    return destIdentifier + " = extractvalue " + structureType.toString()
        + " " + srcIdentifier + ", " + Conversion::toString(fieldSelect);
}

/**
 * Composes string of arguments for inline assembly from given input values.
 *
 * Example output: "=r,ir,ir"(i32 3, i32 -5)
 */
std::string
LLVMIRTools::inlineAssemblyArguments(
    const Operation& operation,
    const Operation::InputOperandVector& inputValues) const {

    return "\"" + constraints(operation)
        + "\"(" + arguments(operation, inputValues) + ")";
}

/**
 * Composes Type list from Operation to describe return value types for
 * asmCallBody().
 *
 */
LLVMIRTools::Type
LLVMIRTools::outputTypeListOf(
    const Operation& operation) const {

    if (operation.numberOfOutputs() == 0) {
        return Type();
    }

    if (operation.numberOfOutputs() > 1) {
        Type typelist;
        for (int i = 0; i < operation.numberOfOutputs(); i++) {
            Type type = singleTypeOf(operation.output(i));
            if (type.isHalfFloat()) {
                // Half types as return type in asm call does not work well.
                // Convert it to integer.
                typelist.addSubType(Type("i16", 2));
            } else {
                typelist.addSubType(type);
            }
        }
        return typelist;
    } else {
        Type type = singleTypeOf(operation.output(0));
        if (type.isHalfFloat()) {
            return Type("i16", 2);
        } else {
            return type;
        }
    }
}

/**
 * Composes string of constraint part of asmCallArgs().
 *
 * Example output: "=r,r,r" (without quotes).
 */
std::string
LLVMIRTools::constraints(
    const Operation& operation) const {

    std::string constraintList;

    for (int i = 0; i <  operation.numberOfOutputs(); i++) {
        TCEString::appendToNonEmpty(constraintList, ",") += "=r";
    }

    for (int i = 0; i < operation.numberOfInputs(); i++) {
        TCEString::appendToNonEmpty(constraintList, ",") += "r";
    }
    return constraintList;
}

/**
 * Writes call argument using inputValues as constants without parentheses.
 *
 * Example output without quotes: "i32 4, float 2.0".
 */
std::string
LLVMIRTools::arguments(
    const Operation& operation,
    const Operation::InputOperandVector& inputValues) const {

    std::string argumentList;
    for (int i = 0; i < operation.numberOfInputs(); i++) {
        TCEString::appendToNonEmpty(argumentList, ", ") +=
            typeOf(operation.input(i)).toString() + " " +
            constant(operation.input(i), inputValues.at(i));
    }
    return argumentList;
}

/**
 * Writes global variable declaration and binds it to new global variable.
 *
 * Use lastGlobalIdentifier() to retrieve the new global variable identifier.
 */
std::string
LLVMIRTools::newGlobalDecl(
        stringCRef typeAndInitialValue,
        stringCRef aligmentValue) {

    std::string globalDecl;

    globalDecl += TCEString::appendInteger(
        "@g", lastReturnedGlobalIdentifier_++);
    globalDecl += std::string(" = ")
        + "internal global "
        + typeAndInitialValue
        + ", align " + aligmentValue;

    return globalDecl;
}

/**
 * Return last generates global identifier created by newGlobalDecl().
 *
 * The Call is valid if newGlobalDecl() has been called.
 */
std::string
LLVMIRTools::lastGlobalIdentifier() const {
    return TCEString::appendInteger("@g", (lastReturnedGlobalIdentifier_ - 1));
}

/**
 * Return free identifier as "%<number>".
 */
std::string
LLVMIRTools::newIdentifier() {
    return std::string("%") + Conversion::toString(
        lastReturnedTempIdentifier_++);
}

/**
 * Return alignment string suitable for Operand used in store and load
 * instructions.
 */
std::string
LLVMIRTools::alignment(
    const Operand& operandInfo) const {
    return std::string("align ") +
        Conversion::toString(alignmentValue(operandInfo));
}

/**
 * Return alignment value suitable for Operand used in store and load
 * instructions.
 */
int
LLVMIRTools::alignmentValue(
    const Operand& operandInfo) const {

    int bitWidth = operandInfo.elementCount()*operandInfo.elementWidth();
    return MathTools::ceil_div(bitWidth, 8);
}

/**
 * Converts signed integer value to LLVM IR constant presentation.
 */
std::string
LLVMIRTools::constant(
    SIntWord value) const {
    return Conversion::toString(value);
}

/**
 * Converts unsigned integer value to LLVM IR constant presentation.
 */
std::string
LLVMIRTools::constant(
    UIntWord value) const {
    return Conversion::toString(value);
}

/**
 * Returns half float in 0xH#### format.
 */
std::string
LLVMIRTools::constant(
    HalfFloatWord value) const {
    return std::string("0xH") +
        Conversion::toHexString(value.getBinaryRep(), 4, false);
}

/**
 * Returns float in hex string representation.
 */
std::string
LLVMIRTools::constant(
    FloatWord value) const {
    return Conversion::doubleToHexString(value);
}

std::string
LLVMIRTools::constant(
    DoubleWord value) const {
    return Conversion::doubleToHexString(value);
}

/**
 * Returns LLVM IR presentation of type using information from Operand.
 *
 * e.g single operand types returns "i32" or "float" and vector operand types
 * return "<4 x i32>".
 */
LLVMIRTools::Type
LLVMIRTools::typeOf(
    const Operand& operandInfo) const {

    if (operandInfo.elementCount() > 1) {
        std::string vectorType("<");
        vectorType += Conversion::toString(operandInfo.elementCount()) +
            " x " + singleTypeOf(operandInfo).toString() + ">";
        return Type(vectorType, alignmentValue(operandInfo));
    } else {
        return singleTypeOf(operandInfo);
    }
}

/**
 * Returns LLVM IR presentation of pointer type.
 *
 * Since LLVM 15 this is an opaque ptr type for all operands
 */
std::string
LLVMIRTools::pointerTypeOf(
    const Operand&) const {

    return "ptr";
}

/**
 * Composes string of pointer to type generated from Operand.
 * 
 * Since LLVM 15 this is an opaque ptr type for all operands
 */
std::string
LLVMIRTools::pointerOfTo(
        const Operand&,
        stringCRef) const {

    return "ptr";
}

/**
 * Same as TypeOf() but returns underlying Type.
 *
 * e.g. For vector operand types returns the base type instead of vector-type.
 */
LLVMIRTools::Type
LLVMIRTools::singleTypeOf(
    const Operand& operandInfo) const {

    int widthOfType = 0;
    if (operandInfo.elementCount() > 1) {
        widthOfType = operandInfo.elementWidth();
    } else {
        widthOfType = operandInfo.width();
    }

    widthOfType = MathTools::roundUpToPowerTwo(std::max(widthOfType, 8));

    switch (operandInfo.type()) {
        case Operand::SINT_WORD:
        case Operand::UINT_WORD:
        case Operand::RAW_DATA:
            return Type(
                "i" + Conversion::toString(widthOfType),
                widthOfType/8);
            //return Type("i32", 4);
            break;
        case Operand::HALF_FLOAT_WORD:
            return Type("half", 2);
            break;
        case Operand::FLOAT_WORD:
            return Type("float", 4);
            break;
        case Operand::DOUBLE_WORD:
            return Type("double", 8);
            break;
        case Operand::BOOL:
            // Return type for BOOL as byte.
            return Type("i8", 1);
            break;
        default:
            assert(false && "Unknown Operand type.");
    }
    assert(false);
    return Type();
}

/**
 * Converts SimValue to LLVM IR constant presentation. Deducts type from
 * Operand.
 */
std::string
LLVMIRTools::constant(
    const Operand& operandInfo,
    const SimValue& value) const {

    TCEString constantStr;
    union {
        SIntWord si;
        UIntWord ui;
    } conv;

    bool isVector = operandInfo.elementCount() > 1;
    constantStr.appendIf(isVector, "<");

    for (int i = 0; i < operandInfo.elementCount(); i++) {
        if (isVector) {
            constantStr.appendIf(i > 0, ", ");
            constantStr.append(singleTypeOf(operandInfo).toString());
            constantStr.append(" ");
        }
        switch (operandInfo.type()) {
            case Operand::SINT_WORD:
                conv.ui = value.element(i, operandInfo.elementWidth());
                conv.si = MathTools::signExtendTo(
                    conv.si, operandInfo.elementWidth());
                constantStr.append(constant(conv.si));
                break;
            case Operand::UINT_WORD:
            case Operand::RAW_DATA:
                constantStr.append(
                    constant(value.element(i, operandInfo.elementWidth())));
                break;
            case Operand::HALF_FLOAT_WORD:
                constantStr.append(constant(value.halfFloatElement(i)));
                break;
            case Operand::FLOAT_WORD:
                constantStr.append(constant(value.floatElement(i)));
                break;
            case Operand::DOUBLE_WORD:
                constantStr.append(constant(value.doubleFloatElement(i)));
                break;
            case Operand::BOOL:
                constantStr.append(constant(value.uIntWordElement(i)));
                break;
            default:
                assert(false && "Unknown Operand type.");
        }
    }
    constantStr.appendIf(isVector, ">");
    return constantStr;
}

/**
 * Returns empty string if one of the arguments is empty. Otherwise return
 * concatenated string of the arguments.
 */
std::string
LLVMIRTools::bothNonEmpty(
    stringCRef str1,
    stringCRef str2) const {
    if (str1.empty() || str2.empty()) {
        return "";
    } else {
        return str1 + str2;
   }

}

/**
 * Resets
 */
void
LLVMIRTools::resetIdentifiers() {
    lastReturnedTempIdentifier_ = 0;
    lastReturnedGlobalIdentifier_ = 0;
}

