/**
 * @file AddCostFunctionPluginCmd.hh
 *
 * Declaration of AddCostFunctionPluginCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_COST_FUNCTION_PLUGIN_HH
#define TTA_ADD_COST_FUNCTION_PLUGIN_HH

#include "GUICommand.hh"

/**
 * Command for adding a new cost function plugin to a HDB.
 */
class AddCostFunctionPluginCmd : public GUICommand {
public:
    AddCostFunctionPluginCmd();
    virtual ~AddCostFunctionPluginCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddCostFunctionPluginCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
