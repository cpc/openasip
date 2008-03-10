/**
 * @file MachineCanvasOptions.hh
 *
 * Declaration of MachineCanvasOptions class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_CANVAS_OPTIONS_HH
#define TTA_MACHINE_CANVAS_OPTIONS_HH

#include <string>
#include "Options.hh"

/**
 * Options for MachineCanvas.
 */
class MachineCanvasOptions : public Options {
public:
    MachineCanvasOptions();
    virtual ~MachineCanvasOptions();

    /// Option name for the unit info string visibility flag.
    static const std::string SHOW_UNIT_INFO_STRING;
};

#endif
