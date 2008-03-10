/**
 * @file SettingCommand.hh
 *
 * Declaration of SettingCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SETTING_COMMAND
#define TTA_SETTING_COMMAND

#include <string>
#include <map>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreter.hh"
#include "StringTools.hh"

/**
 * Implementation of the "setting" command of the Simulator Control Language.
 */
class SettingCommand : public SimControlLanguageCommand {
public:
    SettingCommand();
    virtual ~SettingCommand();

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual std::string helpText() const;

    /**
     * An interface representing a simulator setting.
     */
    class SimulatorSetting {
    public:
        virtual bool setValue(
            SimulatorInterpreter& interpreter, 
            SimulatorFrontend& simFront,
            const DataObject& newValue) = 0;

        virtual std::string valueAsString() = 0;

        virtual std::string description() = 0;

        virtual ~SimulatorSetting() {}
    }; 

protected:
    /// container for simulator settings indexed by name
    typedef std::map<std::string, SimulatorSetting*> SettingMap;
    /// storage for simulator settings
    SettingMap settings_;
};

#include "SettingCommand.icc"

#endif
