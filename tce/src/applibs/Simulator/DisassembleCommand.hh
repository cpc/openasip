/**
 * @file DisassembleCommand.hh
 *
 * Declaration of DisassembleCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLE_COMMAND
#define TTA_DISASSEMBLE_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"


/**
 * Implementation of the "disassemble" command of the 
 * Simulator Control Language.
 */
class DisassembleCommand : public SimControlLanguageCommand {
public:
    DisassembleCommand();
    virtual ~DisassembleCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
