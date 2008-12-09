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
 * @file GuardField.hh
 *
 * Declaration of GuardField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_GUARD_FIELD_HH
#define TTA_GUARD_FIELD_HH

#include <vector>
#include "InstructionField.hh"

class GPRGuardEncoding;
class FUGuardEncoding;
class UnconditionalGuardEncoding;
class MoveSlot;

/**
 * GuardField class represents the guard field of the move slot.
 *
 * The guard field defines the guard expressions that control whether
 * a data transport is completed or cancelled. A GuardField instance
 * contains the control codes that identify the guard expressions
 * supported by a given move slot. GuardField is an instruction field,
 * hence it is derived from class InstructionField. GuardField
 * instances cannot exist alone; they belong to a move slot and are
 * registered to a MoveSlot object at construction time.
 */
class GuardField : public InstructionField {
public:
    GuardField(MoveSlot& parent)
	throw (ObjectAlreadyExists);
    GuardField(const ObjectState* state, MoveSlot& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~GuardField();

    MoveSlot* parent() const;

    void addGuardEncoding(GPRGuardEncoding& encoding)
	throw (ObjectAlreadyExists);
    void removeGuardEncoding(GPRGuardEncoding& encoding);

    void addGuardEncoding(FUGuardEncoding& encoding)
	throw (ObjectAlreadyExists);
    void removeGuardEncoding(FUGuardEncoding& encoding);

    void addGuardEncoding(UnconditionalGuardEncoding& encoding)
        throw (ObjectAlreadyExists);
    void removeUnconditionalGuardEncoding(
        UnconditionalGuardEncoding& encoding);

    int gprGuardEncodingCount() const;
    GPRGuardEncoding& gprGuardEncoding(int index) const
	throw (OutOfRange);
    bool hasGPRGuardEncoding(
	const std::string& regFile,
	int index,
	bool inverted) const;
    GPRGuardEncoding& gprGuardEncoding(
	const std::string& regFile,
	int index,
	bool inverted) const;

    int fuGuardEncodingCount() const;
    FUGuardEncoding& fuGuardEncoding(int index) const
	throw (OutOfRange);
    bool hasFUGuardEncoding(
	const std::string& fu,
	const std::string& port,
	bool inverted) const;
    FUGuardEncoding& fuGuardEncoding(
	const std::string& fu,
	const std::string& port,
	bool inverted) const;

    bool hasUnconditionalGuardEncoding(bool inverted) const;
    UnconditionalGuardEncoding& unconditionalGuardEncoding(bool inverted) 
        const;

    // methods inherited from InstructionField class
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int) const
	throw (OutOfRange);
    virtual int width() const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for guard field.
    static const std::string OSNAME_GUARD_FIELD;

private:
    /// A container type for GPR guard encodings.
    typedef std::vector<GPRGuardEncoding*> GPRGuardTable;
    /// A container type for FU guard encodings.
    typedef std::vector<FUGuardEncoding*> FUGuardTable;

    void deleteGPRGuardEncodings();
    void deleteFUGuardEncodings();
    void deleteUnconditionalGuardEncodings();
    bool isAssigned(unsigned int encoding) const;

    /// GPR guard encodings.
    GPRGuardTable gprGuards_;
    /// FU guard encodings.
    FUGuardTable fuGuards_;
    /// Unconditional guard encoding for always-true expression.
    UnconditionalGuardEncoding* alwaysTrue_;
    /// Unconditional guard encoding for always-false expression.
    UnconditionalGuardEncoding* alwaysFalse_;
};

#endif


