/**
 * @file OSEdConstants.hh
 *
 * Declaration of OSEdConstants class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef OSED_CONSTANTS_HH
#define OSED_CONSTANTS_HH

#include <string>
#include <wx/string.h>
#include <wx/wx.h>
#include <wx/font.h>
#include <wx/treectrl.h>

/**
 * Holds all the constants for OSEd.
 */
class OSEdConstants {
public:

    /// Quit command name.
    static const std::string CMD_NAME_QUIT;
    /// About command name.
    static const std::string CMD_NAME_ABOUT;
    /// Operation properties command name.
    static const std::string CMD_NAME_PROPERTIES;
    /// Add module command name.
    static const std::string CMD_NAME_ADD_MODULE;
    /// Add operation command name.
    static const std::string CMD_NAME_ADD_OPERATION;
    /// Simulate command name.
    static const std::string CMD_NAME_SIMULATE;
    /// Option command name.
    static const std::string CMD_NAME_OPTIONS;
    /// Build command name.
    static const std::string CMD_NAME_BUILD;
    /// Build all command name.
    static const std::string CMD_NAME_BUILD_ALL;
    /// User manual command name.
    static const std::string CMD_NAME_USER_MANUAL;
    /// Remove module command name.
    static const std::string CMD_NAME_REMOVE_MODULE;
    /// Modify behavior command name.
    static const std::string CMD_NAME_MODIFY_BEHAVIOR;
    /// Remove operation command name.
    static const std::string CMD_NAME_REMOVE_OPERATION;
    /// Investigate memory command name.
    static const std::string CMD_NAME_MEMORY;
    
    /// Configuration file name.
    static const std::string CONF_FILE_NAME;
    /// Name of the user manual.
    static const std::string USER_MANUAL_NAME;
    /// Name of the behavior template file name.
    static const std::string BEHAVIOR_TEMPLATE_FILE_NAME;
    
    /// Default editor name.
    static const std::string DEFAULT_EDITOR;

    /**
     * All the command ids.
     */
    enum CommandID {
        CMD_QUIT = 1,          ///< Quit command id.
        CMD_ADD_MODULE,        ///< Add module command id.
        CMD_ADD_OPERATION,     ///< Add operation command id.
        CMD_PROPERTIES,        ///< Operation properties command id.
        CMD_REMOVE_MODULE,     ///< Remove module command id.
        CMD_REMOVE_OPERATION,  ///< Remove operation command id.
        CMD_MODIFY_BEHAVIOR,   ///< Modify operation behavior command id.
        CMD_BUILD,             ///< Build command id.
        CMD_BUILD_ALL,         ///< Build all command id.
        CMD_SIMULATE,          ///< Simulate command id.
        CMD_ABOUT,             ///< About command id.
        CMD_OPTIONS,           ///< Option command id.
        CMD_USER_MANUAL,       ///< User manual command id.
        CMD_MEMORY,            ///< Memory command id.
        CMD_TREE_ITEM,         ///< Id for tree events.
        CMD_INFO_VIEW          ///< Id for list events.
    };
    
    /// The name of the application.
    static const wxString APPLICATION_NAME;
    
    /// Bold font. 
    static const wxFont* BOLD;

    /// Default column width.
    static const int DEFAULT_COLUMN_WIDTH;

    /// Name of the logo.
    static const std::string LOGO_NAME;
    
    /// Copyright string
    static const wxString OSED_COPYRIGHT;
};

#endif
