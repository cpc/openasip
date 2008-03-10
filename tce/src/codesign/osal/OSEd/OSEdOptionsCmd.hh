/**
 * @file OSEdOptionsCmd.hh
 *
 * Declaration of OSEdOptionsCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_OPTIONS_CMD_HH
#define TTA_OSED_OPTIONS_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command for modifying options.
 */
class OSEdOptionsCmd : public GUICommand {
public:
	OSEdOptionsCmd();
	virtual ~OSEdOptionsCmd();

	virtual int id() const;
	virtual GUICommand* create() const;
	virtual bool Do();
	virtual bool isEnabled();
	virtual std::string icon() const;

private:
	/// Copying not allowed.
	OSEdOptionsCmd(const OSEdOptionsCmd&);
	/// Assignment not allowed.
	OSEdOptionsCmd& operator=(const OSEdOptionsCmd&);
};

#endif
