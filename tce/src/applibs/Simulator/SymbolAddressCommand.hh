/**
 * @file SymbolAddressCommand.hh
 *
 * Declaration of SymbolAddressCommand class
 *
 * @author Pekka J‰‰skel‰inen 2008 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SYMBOL_ADDRESS_COMMAND
#define TTA_SYMBOL_ADDRESS_COMMAND

#include <string>
#include <vector>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"

/**
 * Implementation of the "symbol_address" command of the Simulator 
 * Control Language.
 */
class SymbolAddressCommand : public SimControlLanguageCommand {
public:
    SymbolAddressCommand();
    virtual ~SymbolAddressCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;
};
#endif
