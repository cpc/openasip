/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file SettingCommand.hh
 *
 * Declaration of SettingCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
