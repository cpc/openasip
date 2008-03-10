/**
 * @file ProximSimulatorSettingsCmd.hh
 *
 * Declaration of ProximSimulatorSettingsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_SIMULATOR_SETTINGS_CMD
#define TTA_PROXIM_SIMULATOR_SETTINGS_CMD

#include "GUICommand.hh"

/**
 * Command for opening the simulator settings dialog in Proxim.
 */
class ProximSimulatorSettingsCmd : public GUICommand {
public:
    ProximSimulatorSettingsCmd();
    virtual ~ProximSimulatorSettingsCmd();
    virtual bool Do();
    virtual bool isEnabled();
    virtual int id() const;
    virtual std::string icon() const;
    ProximSimulatorSettingsCmd* create() const;
};

#endif
