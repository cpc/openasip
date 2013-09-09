/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file OSEdTextGenerator.hh
 *
 * Declaration of OSEdTextGenerator class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_OSED_TEXT_GENERATOR_HH
#define TTA_OSED_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Provides static texts visible in the OSEd user interface.
 */
class OSEdTextGenerator : public Texts::TextGenerator {
public:
    static OSEdTextGenerator& instance();
    static void destroy();

    /**
     * Text ids.
     */
    enum {
        // text labels
        TXT_LABEL_OPERATION_NAME = Texts::LAST__, ///< Operation name label.
        TXT_LABEL_MODULE_NAME,            ///< Module name label.
        TXT_LABEL_HAS_BEHAVIOR,           ///< Has behavior label.
        TXT_LABEL_NO_BEHAVIOR,            ///< No behavior label.
        TXT_LABEL_READY,                  ///< Ready label.
        TXT_LABEL_UPDATE,                 ///< Update label.
        TXT_LABEL_EDITOR,                 ///< Editor label.
        TXT_LABEL_FORMAT,                 ///< Format label.
        TXT_LABEL_CLOCK,                  ///< Clock label.
        TXT_LABEL_PC,                     ///< Program counter label.
        TXT_LABEL_RA,                     ///< Return address label.
        TXT_LABEL_SYS_CALL_HANDLER,       ///< Sys call handler label.
        TXT_LABEL_SYS_CALL_NUMBER,        ///< Sys call number label.
        TXT_LABEL_MODE,                   ///< Mode label.
        TXT_LABEL_END,                    ///< Memory end label.
        TXT_LABEL_START,                  ///< Memory start label.

        // button labels
        TXT_BUTTON_MODIFY,                ///< Modify button label.
        TXT_BUTTON_OPEN,                  ///< Open button label.
        TXT_BUTTON_TRIGGER,               ///< Trigger button label.
        TXT_BUTTON_LATE_RESULT,           ///< Late result button label.
        TXT_BUTTON_ADVANCE_LOCK,          ///< Advance Lock button label.
        TXT_BUTTON_SAVE,                  ///< Save button label.
        TXT_BUTTON_UNDO,                  ///< Undo button label.
        TXT_BUTTON_DELETE,                ///< Delete button label.
        TXT_BUTTON_NEW,                   ///< New button label.
        TXT_BUTTON_BROWSE,                ///< Browse button label.
        TXT_BUTTON_RESET,                 ///< Reset button label.
        TXT_BUTTON_CLOSE,                 ///< Close button label.
        TXT_BUTTON_SHOW_REGISTERS,        ///< Show registers button label.
        TXT_BUTTON_HIDE_REGISTERS,        ///< Hide registers button label.
        TXT_BUTTON_UPDATE,                ///< Update button label.

        // checkbox texts
        TXT_CHECKBOX_READS_MEMORY,        ///< Reads memory label.
        TXT_CHECKBOX_WRITES_MEMORY,       ///< Writes memory label.
        TXT_CHECKBOX_CAN_TRAP,            ///< Can trap label.
        TXT_CHECKBOX_HAS_SIDE_EFFECTS,    ///< Has side effects label.
        TXT_CHECKBOX_CLOCKED,             ///< Clocked label.
        TXT_CHECKBOX_MEM_DATA,            ///< Memory data label.
        TXT_CHECKBOX_MEM_ADDRESS,         ///< Memory address label.

        // column headers
        TXT_COLUMN_OPERATION,             ///< Operation column header.
        TXT_COLUMN_OPERAND,               ///< Operand column header.
        TXT_COLUMN_TYPE,                  ///< Type column header.
        TXT_COLUMN_VALUE,                 ///< Value column header.
        TXT_COLUMN_SEARCH_PATHS,          ///< Search path column header.
        TXT_COLUMN_MODULES,               ///< Module column header.
        TXT_COLUMN_OPERATIONS,            ///< Operations column header.
        TXT_COLUMN_PROPERTY,              ///< Property column header.
        TXT_COLUMN_OPERAND_VALUE,         ///< Operand value column header.
        TXT_COLUMN_ELEMENT_WIDTH,         ///< Operand element width header.
        TXT_COLUMN_ELEMENT_COUNT,         ///< Operand element count header.

        // dialog titles
        TXT_PROPERTY_DIALOG_TITLE,        ///< Property dialog title.
        TXT_INPUT_OPERAND_DIALOG_TITLE,   ///< Input operand dialog title.
        TXT_OUTPUT_OPERAND_DIALOG_TITLE,  ///< Output operand dialog title.
        TXT_ADD_MODULE_DIALOG_TITLE,      ///< Add module dialog title.
        TXT_SIMULATE_DIALOG_TITLE,        ///< Simulate dialog title.
        TXT_OPTIONS_DIALOG_TITLE,         ///< Options dialog title.
        TXT_BUILD_RESULT_DIALOG_TITLE,    ///< Build result dialog title.
        TXT_USER_MANUAL_TITLE,            ///< User manual dialog title.
        TXT_XML_RESULT_DIALOG_TITLE,      ///< XML result dialog title.
        TXT_ABOUT_DIALOG_TITLE,           ///< About dialog title.
        TXT_SIMULATION_INFO_DIALOG_TITLE, ///< Simulation info dialog title.
        TXT_MEMORY_DIALOG_TITLE,          ///< Memory dialog title.

        // box sizer labels
        TXT_BOX_AFFECTS,                  ///< Affects sizer label.
        TXT_BOX_AFFECTED_BY,              ///< Affected by sizer label.
        TXT_BOX_INPUT_OPERANDS,           ///< Input operand sizer label.
        TXT_BOX_OUTPUT_OPERANDS,          ///< Output operand sizer label.
        TXT_BOX_CAN_SWAP,                 ///< Can swap sizer label.
        TXT_BOX_INPUT_VALUES,             ///< Input values sizer label.
        TXT_BOX_OUTPUT_VALUES,            ///< Output values sizer label.

        // error texts
        TXT_ERROR_NO_NAME,                ///< Error when no name is given.
        TXT_ERROR_CAN_NOT_MODIFY,         ///< Error when can not modify.
        TXT_ERROR_CAN_NOT_CREATE_MOD,     ///< Error when can not create 
                                          ///< module.

        TXT_ERROR_CAN_NOT_CREATE_OP,      ///< Error when can not create
                                          ///< operation.

        TXT_ERROR_NO_EDITOR,              ///< Error when no editor is given.
        TXT_ERROR_OPEN,                   ///< Error when opening fails.
        TXT_ERROR_CAN_NOT_SAVE,           ///< Error when options can not be
                                          ///< saved.
        
        TXT_ERROR_NON_EXISTING_OPERATION, ///< Error when operation does not
                                          ///< exist.
        TXT_ERROR_TRIGGER_FAILED,         ///< Error when trigger fails.
        TXT_ERROR_CAN_NOT_REMOVE_OPERATION, ///< Error when operation can not
                                            ///< be removed.
        
        TXT_ERROR_MODULE_EXISTS,          ///< Module exists error.
        TXT_ERROR_OPERATION_EXISTS,       ///< Operation exists error.
        TXT_ERROR_NO_HANDLER,             ///< Error when custom command handler
                                          ///< is not found.
        TXT_ERROR_MOD_NAME,               ///< Module name is erronous.
 
        // message
        TXT_MSG_BUILD_OK,                 ///< Build ok message.
		
        // about text
        TXT_ABOUT,                        ///< About text.

        // menu item labels
        TXT_MENU_OPTIONS,                 ///< Option menu label.
        TXT_MENU_QUIT,                    ///< Quit menu label.
        TXT_MENU_FILE,                    ///< File menu label.
        TXT_MENU_ADD_OPERATION,           ///< Add operation menu label.
        TXT_MENU_REMOVE_OPERATION,        ///< Remove operation menu label.
        TXT_MENU_ADD_MODULE,              ///< Add module menu label.
        TXT_MENU_MODIFY_PROPERTIES,       ///< Modify properties menu label.
        TXT_MENU_BUILD,                   ///< Build menu label.
        TXT_MENU_BUILD_ALL,               ///< Build all menu label.
        TXT_MENU_SIMULATE,                ///< Simulate menu label.
        TXT_MENU_TOOLS,                   ///< Tools menu label.
        TXT_MENU_ABOUT,                   ///< About menu label.
        TXT_MENU_USER_MANUAL,             ///< User manual menu label.
        TXT_MENU_HELP,                    ///< Help menu label.
        TXT_MENU_REMOVE_MODULE,           ///< Remove module menu label.
        TXT_MENU_MODIFY_BEHAVIOR,         ///< Modify behavior menu label.
        TXT_MENU_MEMORY,                  ///< Memory menu label.

        // row texts
        TXT_ROW_NAME,                     ///< Name row label.
        TXT_ROW_DESCRIPTION,              ///< Description row label.
        TXT_ROW_INPUTS,                   ///< Inputs row label.
        TXT_ROW_OUTPUTS,                  ///< Outputs row label.
        TXT_ROW_READS_MEMORY,             ///< Reads memory row label.
        TXT_ROW_TRUE,                     ///< True text.
        TXT_ROW_FALSE,                    ///< False text.
        TXT_ROW_WRITES_MEMORY,            ///< Writes memory row label.
        TXT_ROW_CAN_TRAP,                 ///< Can trap row label.
        TXT_ROW_HAS_SIDE_EFFECTS,         ///< Has side effects row label.
        TXT_ROW_CLOCKED,                  ///< Clocked row label.
        TXT_ROW_AFFECTED_BY,              ///< Affected by row label.
        TXT_ROW_AFFECTS,                  ///< Affects row label.
        TXT_ROW_INPUT_OPERANDS,           ///< Input operands row label.
        TXT_ROW_OUTPUT_OPERANDS,          ///< Output operands row label.
        TXT_ROW_ID,                       ///< Id row label.
        TXT_ROW_YES,                      ///< Yes text.
        TXT_ROW_NO,                       ///< No text.
        TXT_ROW_TYPE,                     ///< Type row label.
        TXT_ROW_MEMORY_ADDRESS,           ///< Memory address row label.
        TXT_ROW_CAN_SWAP,                 ///< Can swap row label.
        TXT_ROW_MEMORY_DATA,              ///< Memory data row label.
        TXT_ROW_HAS_BEHAVIOR,             ///< Has behavior row label.
        
        // questions
        TXT_QUESTION_REMOVE_MODULE,       ///< Remove module question.
        TXT_QUESTION_REMOVE_BEHAVIOR_FILE,///< Remove behavior question.
        TXT_QUESTION_SAVE_PROPERTIES,     ///< Save properties question.
        TXT_QUESTION_REMOVE_OPERATION,    ///< Remove operation question.

        TXT_STATUS_PATH_SELECTED,         ///< Status bar text when path 
                                          ///< is selected.
        TXT_STATUS_MODULE_SELECTED,       ///< Status bar text when module
                                          ///< is selected.
        TXT_STATUS_OPERATION_SELECTED,    ///< Status bar text when operation
                                          ///< is selected.
        TXT_STATUS_COMPILING,             ///< Status bar text when compiling.
        TXT_STATUS_COMPILE_SUCCESS,       ///< Status bar text when compiling
                                          ///< was successful.
        TXT_STATUS_COMPILE_FAILED         ///< Status bar text when compilation
                                          ///< failed.
    };

private:
    OSEdTextGenerator();
    virtual ~OSEdTextGenerator();
	
    /// Static instance of text generator.
    static OSEdTextGenerator* instance_;
};

#endif
