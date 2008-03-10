/**
 * @file DropDownMenu.hh
 *
 * Declaration of DropDownMenu class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DROP_DOWN_MENU_HH
#define TTA_DROP_DOWN_MENU_HH

#include <wx/wx.h>

/**
 * Class that models drop down menu.
 *
 * Drop down menu is shown, when user clicks right mouse button over
 * tree item or list item in main window.
 */
class DropDownMenu : public wxMenu {
public:

    /**
     * Type of the drop down menu.
     */
    enum MenuType {
        MENU_PATH,      ///< Path menu.
        MENU_MODULE,    ///< Module menu.
        MENU_OPERATION  ///< Operation menu.
    };

    explicit DropDownMenu(MenuType type);
    virtual ~DropDownMenu();

private:
    /// Copying not allowed.
    DropDownMenu(const DropDownMenu&);
    /// Assignment not allowed.
    DropDownMenu& operator=(const DropDownMenu&);
    
    void onMenuEvent(wxCommandEvent& event);
    void updateMenu();

    DECLARE_EVENT_TABLE()
};

#endif
