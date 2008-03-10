/**
 * @file CompiledSimSettingCommand.hh
 *
 * Declaration of CompiledSimSettingCommand class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_SETTING_COMMAND
#define COMPILED_SIM_SETTING_COMMAND

#include "SettingCommand.hh"

/**
 * Implementation of the "setting" command for the compiled simulator
 */
class CompiledSimSettingCommand : public SettingCommand {
public:
    CompiledSimSettingCommand();
    virtual ~CompiledSimSettingCommand();
};

#endif
