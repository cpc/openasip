/**
 * @file DialogPosition.hh
 *
 * Declaration of DialogPosition class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DIALOG_POSITION_HH
#define TTA_DIALOG_POSITION_HH

#include <wx/wx.h>
#include <map>

/**
 * Class that holds the positions of dialogs.
 */
class DialogPosition {
public:
    /**
     * Enumeration of all dialogs in application.
     */
    enum Dialogs {
        DIALOG_PROPERTIES,        ///< Operation properties dialog.
        DIALOG_SIMULATE,          ///< Simulation dialog.
        DIALOG_INPUT_OPERAND,     ///< Input operand dialog.
        DIALOG_OUTPUT_OPERAND,    ///< Output operand dialog.
        DIALOG_RESULT,            ///< Result dialog.
        DIALOG_OPTIONS,           ///< Options dialog.
        DIALOG_ADD_MODULE,        ///< Add module dialog.
        DIALOG_SIMULATION_INFO,   ///< Simulation info dialog.
        DIALOG_MEMORY,            ///< Memory dialog.
        DIALOG_ABOUT              ///< About dialog.
    };

    static wxPoint getPosition(Dialogs dialog);
    static void setPosition(Dialogs dialog, wxPoint point);

private:
    /// Container for dialog positions.
    typedef std::map<Dialogs, wxPoint> PositionMap;

    /// Copying not allowed.
    DialogPosition(const DialogPosition&);
    /// Assignment not allowed.
    DialogPosition& operator=(const DialogPosition&);

    DialogPosition();
    ~DialogPosition();

    /// Contains all dialog positions.
    static PositionMap positions_;
};

#endif
