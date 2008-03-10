/**
 * @file GuardField.hh
 *
 * Declaration of GuardField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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


