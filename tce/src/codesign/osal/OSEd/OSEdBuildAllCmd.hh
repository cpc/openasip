/**
 * @file OSEdBuildAllCmd.hh
 *
 * Declaration of OSEdBuildAllCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_BUILD_ALL_CMD_HH
#define TTA_OSED_BUILD_ALL_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command that build all modules.
 */
class OSEdBuildAllCmd : public GUICommand {
public:
    OSEdBuildAllCmd();
    virtual ~OSEdBuildAllCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdBuildAllCmd(const OSEdBuildAllCmd&);
    /// Assignment not allowed.
    OSEdBuildAllCmd& operator=(const OSEdBuildAllCmd&);
};

#endif
