/**
 * @file OSEdBuildCmd.hh
 *
 * Definition of OSEdBuildCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_BUILD_CMD_HH
#define TTA_OSED_BUILD_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command that builds a module.
 */
class OSEdBuildCmd : public GUICommand {
public:
    OSEdBuildCmd();
    virtual ~OSEdBuildCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdBuildCmd(const OSEdBuildCmd&);
    /// Assignment not allowed.
    OSEdBuildCmd& operator=(const OSEdBuildCmd&);
};

#endif
