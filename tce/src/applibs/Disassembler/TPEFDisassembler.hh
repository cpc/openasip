/**
 * @file TPEFDisassembler.hh
 *
 * Declaration of TPEFDisassembler class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TPEF_DISASSEMBLER_HH
#define TTA_TPEF_DISASSEMBLER_HH

#include <vector>

#include "BaseType.hh"
#include "Disassembler.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"
#include "Binary.hh"

class DisassemblyInstruction;
class DisassemblyElement;

/**
 * TPEF Disassembler.
 *
 * Retrievs data from TPEF and builds Disassembler instructions
 * out of TPEF hierarchy.
 *
 * Contains internal cache to speedup instruction fetching, so if
 * instruction ordering of TPEF is changed cache will not be
 * valid anymore and must be cleared.
 *
 * NOTE: Class disassembles only the first of code sections in binary.
 */
class TPEFDisassembler : public Disassembler {
public:
    TPEFDisassembler(const TPEF::Binary &aTpef);
    virtual ~TPEFDisassembler();

    virtual DisassemblyInstruction* createInstruction(
	Word instructionIndex) const;

    virtual Word startAddress() const;
    virtual Word instructionCount() const;

    void clearCache() const;

protected:
    typedef std::pair<Word,Word> ImmediateKey;
    typedef std::map<ImmediateKey, TPEF::ImmediateElement*> ImmediateMap;

    void initCache() const;

    DisassemblyElement* createDisassemblyElement(
	TPEF::MoveElement::FieldType type, Word unit, Word index,
	ImmediateMap &immediateMap) const;

private:
    /// Binary where data for disassembler is retrieved.
    const TPEF::Binary *tpef_;

    /// Cache of starting elements of instructions.
    mutable std::vector<Word> instructionStartCache_;
};
#endif
