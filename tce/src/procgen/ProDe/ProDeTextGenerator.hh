/**
 * @file ProDeTextGenerator.hh
 *
 * Declaration of the ProDeTextGenerator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@.cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_TEXT_GENERATOR_HH
#define TTA_PRODE_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

class wxWindow;

/**
 * Provides static texts visible in the ProDe user interface.
 *
 * The class is implemented using the 'singleton' design pattern.
 * Only one instance of this class is created, and the instance
 * can be accessed using the instance() method.
 * This class only provides ProDe-specific texts. Texts common
 * to all TCE-guis are provided by the GUITextGenerator class.
 */
class ProDeTextGenerator : public Texts::TextGenerator {
public:
    static ProDeTextGenerator* instance();
    static void destroy();

    /// Ids for the generated texts.
    enum {
        TXT_LABEL_NAME = Texts::LAST__, ///< Label for component name widget.
        TXT_LABEL_WIDTH,                ///< Label for bit width widget.
	TXT_LABEL_MAU,                  ///< Label for min. addressable unit.
        TXT_LABEL_ADDRESS_SPACE,        ///< Label for address spave selector.
        TXT_LABEL_SOCKET,               ///< Label for socket selection.
        TXT_LABEL_INPUT_SOCKET,         ///< Label for input socket selector.
        TXT_LABEL_OUTPUT_SOCKET,        ///< Label for output socket selector.
        TXT_LABEL_ENABLED,              ///< Label for 'enable' checkbox.
        TXT_LABEL_TRUE_GUARD,           ///< Label for 'always true guard'.
        TXT_LABEL_FALSE_GUARD,          ///< Label for 'always false guard'.
        TXT_LABEL_EXTENSION,            ///< Label for 'extension' radiobox.
        TXT_LABEL_TYPE,                 ///< Label for type widget.
        TXT_LABEL_SIZE,                 ///< Label for size widget.
        TXT_LABEL_MAX_WRITES,           ///< Label for Max-Writes widget.
        TXT_LABEL_MAX_READS,            ///< Label for Max-Reads widget.
        TXT_LABEL_SLOT,                 ///< Label for slot widget.
        TXT_LABEL_DESTINATION,          ///< Label for destination widget.
        TXT_LABEL_CYCLES,               ///< Label for cycles widget.
        TXT_LABEL_PORT,                 ///< Label for port widget.
        TXT_LABEL_OPERAND,              ///< Label for operand widget.
        TXT_LABEL_OPCODE_PORT,          ///< Label for sets opcode widget.
        TXT_LABEL_TRIGGERS,             ///< Label for 'triggers' checkbox.
        TXT_LABEL_OPPOSITE_BRIDGE,      ///< Label for opposite bridge name.
        TXT_LABEL_INPUT_BUS,            ///< Label for input bus widget.
        TXT_LABEL_OUTPUT_BUS,           ///< Label for output bus widget.
        TXT_LABEL_BIDIRECTIONAL,        ///< Label for bidirectional checkbox.
        TXT_LABEL_FU_NAME,              ///< Label for function unit name.
        TXT_LABEL_PORT_NAME,            ///< Label for port name.
        TXT_LABEL_INVERTED,             ///< Label for inverted checkbox.
        TXT_LABEL_RF_NAME,              ///< Label for register file name.
        TXT_LABEL_REGISTER_INDEX,       ///< Label for register index widget.
        TXT_LABEL_UNDO_LEVELS,          ///< Label for undo levels widget.
        TXT_LABEL_CONTENTS,             ///< Label for contents widget.
        TXT_LABEL_DIRECTION,            ///< Label for direction widget.
        TXT_LABEL_RA_CHOICE,            ///< Label for ret.addr. port choicer.
        TXT_LABEL_GUARD_LATENCY,        ///< Label for guard latency.
        TXT_LABEL_GLOBAL_GUARD_LATENCY, ///< Label for global guard latency.

        TXT_LABEL_BUTTON_DELETE,        ///< Label for &Delete button.
        TXT_LABEL_BUTTON_EDIT,          ///< Label for &Edit... button.
	TXT_BUTTON_ADD_SR_PORT,         ///< Label for GCUdlg add FU port btn.
	TXT_BUTTON_ADD_FU_PORT,         ///< Label for GCUdlg add SR port btn.

        TXT_RADIO_EXTENSION_ZERO,       ///< Label for 'zero' radio button.
        TXT_RADIO_EXTENSION_SIGN,       ///< Label for 'sign' radio button.
        TXT_RADIO_DIRECTION_INPUT,      ///< Label for input radio button.
        TXT_RADIO_DIRECTION_OUTPUT,     ///< Label for output radio button.

        TXT_COLUMN_NAME,                ///< Label for name column in a list.
        TXT_COLUMN_WIDTH,               ///< Label for width column in a list.
        TXT_COLUMN_MAU,                 ///< Label for MAU column in a list.
        TXT_COLUMN_BIT_WIDTH,           ///< Label for bit width column in a list.
        TXT_COLUMN_INVERTED,            ///< Label for inv column in a list.
        TXT_COLUMN_INDEX,               ///< Label for index column in a list.
        TXT_COLUMN_PORT,                ///< Label for port column in a list.
        TXT_COLUMN_SLOT,                ///< Label for slot column in a list.
        TXT_COLUMN_DESTINATION,         ///< Label for destination column.
        TXT_COLUMN_OPERAND,             ///< Label for operand column.
        TXT_COLUMN_TRIGGERS,            ///< Label for T column in a list.
        TXT_COLUMN_COMMAND,             ///< Label for command column.
        TXT_COLUMN_SHORTCUT,            ///< Label for shortcut column.
        TXT_COLUMN_TOOLBAR_BUTTONS,     ///< Label for toolbar buttons column.
        TXT_COLUMN_AVAILABLE_COMMANDS,  ///< Available commands column.
        TXT_COLUMN_MIN_ADDRESS,         ///< Label for min-address column.
        TXT_COLUMN_MAX_ADDRESS,         ///< Label for max-address column.
        TXT_COLUMN_BUS,                 ///< Label for bus column in a list.
        TXT_COLUMN_SEGMENT,             ///< Label for segment column.
	TXT_COLUMN_SRP,                 ///< Label for sp.reg. port column.

        TXT_YES,                        ///< Text 'yes' in lower case.
        TXT_NO,                         ///< Text 'no' in lower case.

        TXT_BUTTON_UP,                  ///< Label for up button.
        TXT_BUTTON_DOWN,                ///< Label for down button.
        TXT_BUTTON_SHIFT_LEFT,          ///< Label for << button.
        TXT_BUTTON_SHIFT_RIGHT,         ///< Label for >> button.
        TXT_BUTTON_INSERT,              ///< Label for insert button.
        TXT_BUTTON_REMOVE,              ///< Label for remove button.
        TXT_BUTTON_ATTACH,              ///< Label for attach button.
        TXT_BUTTON_DETACH,              ///< Label for detach button.

        TXT_GCU_DIALOG_TITLE,           ///< GCU Dialog title.
        TXT_GCU_OPERATIONS_BOX,         ///< GCU Dialog operation box title.
        TXT_GCU_PORTS_BOX,              ///< Port list box title.
        TXT_GCU_DELAY_SLOTS,            ///< Delay slots control label.

        TXT_BUS_DIALOG_TITLE,           ///< Bus Dialog title.
        TXT_BUS_BUS_BOX,                ///< Bus Dialog bus box title.
        TXT_BUS_SEGMENTS_BOX,           ///< Segments box title.
        TXT_BUS_SI_BOX,                 ///< Short immediate box title.
        TXT_BUS_REGISTER_GUARD_BOX,     ///< Register file guards box title.
        TXT_BUS_PORT_GUARD_BOX,         ///< Function unit guards box title.

        TXT_RF_DIALOG_TITLE,            ///< Register file Dialog title.
        TXT_RF_PORTS_BOX,               ///< Ports box title.
        TXT_RF_TYPE_NORMAL,             ///< Register file type: normal.
        TXT_RF_TYPE_VOLATILE,           ///< Register file type: volatile.
        TXT_RF_TYPE_RESERVED,           ///< Register file type: reserved.

        TXT_RF_PORT_DIALOG_TITLE,       ///< Register file port Dialog title.

        TXT_TEMPLATES_DIALOG_TITLE,     ///< Templates dialog title.
        TXT_TEMPLATES_TEMPLATES_BOX,    ///< Instruction Templates bos title.
        TXT_TEMPLATES_SLOTS_BOX,        ///< Template Slots box title.

        TXT_TEMPLATE_SLOT_DIALOG_TITLE, ///< Template Slot dialog title.

        TXT_IMMEDIATE_SLOT_DIALOG_TITLE,///< Immediate Slot dialog title.

        TXT_IU_DIALOG_TITLE,            ///< Immediate unit Dialog title.
        TXT_IU_PORTS_BOX,               ///< Ports box title.
        TXT_IU_TEMPLATES_BOX,           ///< Templates box title.

        TXT_IU_PORT_DIALOG_TITLE,       ///< Immediate unit port dialog title.

        TXT_OPERATION_DIALOG_TITLE,     ///< Operation dialog title.
        TXT_OPERATION_USAGE_BOX,        ///< Pipeline usage box title.
        TXT_OPERATION_RESOURCES_BOX,    ///< Pipeline resources box title.
        TXT_OPERATION_OPERANDS_BOX,     ///< Operands box title.

        TXT_FU_DIALOG_TITLE,            ///< Function unit dialog title.
        TXT_FU_PORTS_BOX,               ///< Ports box title.
        TXT_FU_OPERATIONS_BOX,          ///< Operations box title.

        TXT_FU_PORT_DIALOG_TITLE,       ///< Function unit port dialog title.
        TXT_SR_PORT_DIALOG_TITLE,       ///< Special reg. port dialog title.

        TXT_BRIDGE_DIALOG_TITLE,        ///< Bridge dialog title.

        TXT_FU_GUARD_DIALOG_TITLE,      ///< Function unit guard dialog title.

        TXT_RF_GUARD_DIALOG_TITLE,      ///< Register file guard dialog title.

        TXT_OPTIONS_DIALOG_TITLE,       ///< Options dialog title.
        TXT_OPTIONS_ICONS,              ///< Icons option for toolbar buttons.
        TXT_OPTIONS_TEXT_ICONS,         ///< Text&Icons for toolbar buttons.
        TXT_OPTIONS_TEXT,               ///< Text option for toolbar buttons.

        TXT_ADDRESS_SPACE_DIALOG_TITLE, ///< Address space dialog title.
        TXT_ADDRESS_SPACE_MIN_ADDRESS,  ///< Label for Min-Address box.
        TXT_ADDRESS_SPACE_MAX_ADDRESS,  ///< Label for Max-Address box.

        TXT_ADDRESS_SPACES_DIALOG_TITLE,///< Address spaces dialog title.

        TXT_SOCKET_DIALOG_TITLE,        ///< Socket dialog title.
        TXT_SOCKET_ATTACHED_BOX,        ///< Label for Attached buses box.
        TXT_SOCKET_DETACHED_BOX,        ///< Label for Detached buses box.

        TXT_ABOUT_DIALOG_TITLE,         ///< About dialog title.

        MSG_ERROR,                      ///< Text 'Error' and new line.
        MSG_ERROR_ILLEGAL_NAME,         ///< Error: Illegal component name.
        MSG_ERROR_ONE_GCU,              ///< Error: Multiple control units.
        MSG_ERROR_CANNOT_BRIDGE,        ///< Error: Bridge creation impossible.
        MSG_ERROR_CANNOT_RF_GUARD,      ///< Error: Register file guard.
        MSG_ERROR_CANNOT_FU_GUARD,      ///< Error: Function unit port guard.
        MSG_ERROR_OPCODE_SET,           ///< Error: Operation reads the opcode.
        MSG_ERROR_SAME_NAME,            ///< Error: Same name exists.
        MSG_ERROR_EMPTY_TEMPLATE,       ///< Error: Two empty templates.
        MSG_ERROR_NO_SLOTS_AVAILABLE,   ///< Error: No slots available for IT.
        MSG_ERROR_NO_IUS_AVAILABLE,     ///< Error: No IUs available for IT.
        MSG_ERROR_BUS_IT_SLOT_WIDTH,    ///< Error: Slot width > bus width.
        MSG_ERROR_BRIDGE_NAMES,         ///< Error: Opposing brdgs w/same name.
        MSG_ERROR_NO_OPCODE_OPERAND,    ///< Error: No oprnd for opcode.s.port
        MSG_ERROR_OPERAND_NUM_RESERVED, ///< Error: Operand number reserved.
        MSG_ERROR_CANNOT_ADD,           ///< Error: Something can't be added.
        MSG_ERROR_CANNOT_REMOVE,        ///< Error: Something can't be removed.
        MSG_ERROR_PIPELINE_START,       ///< Error: Pipeline usage start late.
        MSG_ERROR_MAX_READS,            ///< Error: Max-reads too large.
        MSG_ERROR_MAX_WRITES,           ///< Error: Max-writes too large.
        MSG_ERROR_GUARD_EXISTS,         ///< Error: Equal guard exists.
        MSG_ERROR_UNKNOWN_COMMAND_ID,   ///< Error: Unknown command ID.
        MSG_ERROR_LOADING_FILE,         ///< Error: File loading failed.
        MSG_ERROR_NO_IMMEDIATE_UNITS,   ///< Error: No IUs for a template slot.
        
        MSG_WARN_RES_WITHOUT_USAGES,    ///< Warning: Resources without usages

        MSG_CONFIRM_OPERAND_DELETION,   ///< Confirm: operand deletion.

        COMP_MACHINE,                   ///< Text for machine description.
        COMP_A_BUS,                     ///< Name for a bus component.
        COMP_BUS,                       ///< Name for bus component.
        COMP_A_SEGMENT,                 ///< Name for a segment component.
        COMP_SEGMENT,                   ///< Name for segment component.
        COMP_AN_OPERATION,              ///< Name for operation (w/ article).
        COMP_OPERATION,                 ///< Name for operation (w/o article).
        COMP_BRIDGE,                    ///< Name for bridge component.
        COMP_A_BRIDGE,                  ///< Name for bridge (w/ article).
        COMP_RESOURCE,                  ///< Name for resource (w/o article).
        COMP_A_RESOURCE,                ///< Name for resource (w/ article).
        COMP_FUNCTION_UNIT,             ///< Name for FU (w/o article).
        COMP_A_FUNCTION_UNIT,           ///< Name for FU (w/ article).
        COMP_GCU,                       ///< Name for control unit.
        COMP_AN_IMMEDIATE_UNIT,         ///< Immediate unit (w/ article).
        COMP_IMMEDIATE_UNIT,            ///< Immediate unit (w/o article).
        COMP_A_REGISTER_FILE,           ///< Register file (w/ article).
        COMP_REGISTER_FILE,             ///< Register file (w/o article).
        COMP_A_PORT,                    ///< Name for port (w/ article).
        COMP_PORT,                      ///< Name for port (w/o article).
        COMP_A_TEMPLATE,                ///< Name for template (w/ article).
        COMP_TEMPLATE,                  ///< Name for template (w/o article).
        COMP_RESOURCE_USE,              ///< String for resource use.
        COMP_OPERAND_READ,              ///< String for operand read.
        COMP_AN_ADDRESS_SPACE,          ///< Address space (w/ article).
        COMP_ADDRESS_SPACE,             ///< Address space (w/o article).
        COMP_A_SOCKET,                  ///< Name for socket (w/ article).
        COMP_SOCKET,                    ///< Name for socket (w/o article).
        COMP_IMM_SLOT,                  ///< Name for imm. slot (w/o article).
        COMP_AN_IMM_SLOT,               ///< Name for imm. slot (w/ article).

        STATUS_FUNCTION_UNIT,           ///< Status line template for FUs.
        STATUS_REGISTER_FILE,           ///< Status line template for RFs.
        STATUS_IMMEDIATE_UNIT,          ///< Status line template for IUs,
        STATUS_GCU,                     ///< Status line template for GCUs.
        STATUS_BUS,                     ///< Status line template for buses.
        STATUS_SEGMENT,                 ///< Status line template for segments.
        STATUS_SOCKET,                  ///< Status line template for sockets.
        STATUS_BRIDGE,                  ///< Status line template for bridges.
        STATUS_PORT,                    ///< Status line template for ports.
        STATUS_FU_PORT                  ///< Status line template for FU ports.
    };

private:
    ProDeTextGenerator();
    ~ProDeTextGenerator();
    static ProDeTextGenerator* instance_;
};

#endif
