/**
 * @file OSEdInfoView.hh
 *
 * Declaration of OSEdInfoView class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_INFO_VIEW_HH
#define TTA_OSED_INFO_VIEW_HH

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <string>

class Operation;

/**
 * Models an info screen.
 *
 * Info screen shows info about search paths, modules, and operations.
 */
class OSEdInfoView : public wxListCtrl {
public:
    OSEdInfoView(wxWindow* parent);
    virtual ~OSEdInfoView();
    
    void clear();
    void pathView();
    void moduleView(const std::string& name);
    void operationView(const std::string& path, const std::string& mod);
    void operationPropertyView(
        const std::string& opName,
        const std::string& modName,
        const std::string& pathName);

    std::string selectedPath();
    std::string selectedModule();
    std::string selectedOperation();

private:

    void onSelection(wxListEvent& event);
    void onDropDownMenu(wxMouseEvent& event);
    void insertOperationPropertyColumns();
    int  writeStaticPropertiesOfOperation(Operation* op);

    /**
     * Possible modes of info view.
     */
    enum InfoMode {
        MODE_NOMODE,    ///< Initial state.
        MODE_PATH,      ///< Path view.
        MODE_MODULE,    ///< Module view.
        MODE_OPERATION, ///< Operation View.
        MODE_PROPERTY   ///< Operation property view.
    };
    
    /// Mode of the info view.
    InfoMode mode_;

    DECLARE_EVENT_TABLE()
};

#endif
