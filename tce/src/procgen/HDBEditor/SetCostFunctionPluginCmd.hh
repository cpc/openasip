/**
 * @file SetCostFunctionPluginCmd.hh
 *
 * Declaration of SetCostFunctionPluginCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SET_COST_FUNCTION_PLUGIN_CMD_HH
#define TTA_SET_COST_FUNCTION_PLUGIN_CMD_HH

#include "GUICommand.hh"

/**
 * Command for setting cost function plugin for HDB entries.
 */
class SetCostFunctionPluginCmd : public GUICommand {
public:
    SetCostFunctionPluginCmd();
    virtual ~SetCostFunctionPluginCmd();
    virtual bool Do();
    virtual int id() const;
    virtual SetCostFunctionPluginCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();

};

#endif
