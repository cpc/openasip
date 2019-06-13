/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ProDeTextGenerator.cc
 *
 * Implementation of the ProDeTextGenerator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string.h>

#include "ProDeTextGenerator.hh"

using std::string;

ProDeTextGenerator* ProDeTextGenerator::instance_ = NULL;

/**
 * The Constructor.
 */
ProDeTextGenerator::ProDeTextGenerator() {

    addText(TXT_LABEL_NAME, "Name:");
    addText(TXT_LABEL_ENABLED, "Enabled");
    addText(TXT_LABEL_WIDTH, "Width:");
    addText(TXT_LABEL_MAU, "Minimum Addressable Unit:");
    addText(TXT_LABEL_TRUE_GUARD, "Always True Guard");
    addText(TXT_LABEL_FALSE_GUARD, "Always False Guard");
    addText(TXT_LABEL_SOCKET, "Socket:");
    addText(TXT_LABEL_INPUT_SOCKET, "Input Socket:");
    addText(TXT_LABEL_OUTPUT_SOCKET, "Output Socket:");
    addText(TXT_LABEL_ADDRESS_SPACE, "Address Space:");
    addText(TXT_LABEL_EXTENSION, "Extension:");
    addText(TXT_LABEL_TYPE, "Type:");
    addText(TXT_LABEL_SIZE, "Size:");
    addText(TXT_LABEL_MAX_WRITES, "Max-Writes:");
    addText(TXT_LABEL_MAX_READS, "Max-Reads:");
    addText(TXT_LABEL_SLOT, "Slot:");
    addText(TXT_LABEL_DESTINATION, "Destination:");
    addText(TXT_LABEL_CYCLES, "Latency:");
    addText(TXT_LABEL_PORT, "Port:");
    addText(TXT_LABEL_OPERAND, "Operand:");
    addText(TXT_LABEL_OPCODE_PORT, "Opcode setting port:");
    addText(TXT_LABEL_TRIGGERS, "Triggers");
    addText(TXT_LABEL_OPPOSITE_BRIDGE, "Opposite Bridge Name:");
    addText(TXT_LABEL_INPUT_BUS, "Input Bus:");
    addText(TXT_LABEL_OUTPUT_BUS, "Output Bus:");
    addText(TXT_LABEL_BIDIRECTIONAL, "Bidirectional");
    addText(TXT_LABEL_FU_NAME, "Function Unit Name:");
    addText(TXT_LABEL_PORT_NAME, "Port Name:");
    addText(TXT_LABEL_INVERTED, "Inverted");
    addText(TXT_LABEL_RF_NAME, "Register File Name:");
    addText(TXT_LABEL_REGISTER_INDEX, "Register Index:");
    addText(TXT_LABEL_UNDO_LEVELS, "Undo Levels:");
    addText(TXT_LABEL_CONTENTS, "Contents:");
    addText(TXT_LABEL_DIRECTION, "Direction:");
    addText(TXT_LABEL_GUARD_LATENCY, "Guard Latency:");
    addText(TXT_LABEL_GLOBAL_GUARD_LATENCY, "Global Guard Latency:");

    addText(TXT_LABEL_BUTTON_DELETE, "&Delete");
    addText(TXT_LABEL_BUTTON_EDIT, "&Edit...");
    addText(TXT_BUTTON_ADD_FU_PORT, "Add operation port...");
    addText(TXT_BUTTON_ADD_SR_PORT, "Add special register port...");

    addText(TXT_RADIO_EXTENSION_ZERO, "Zero");
    addText(TXT_RADIO_EXTENSION_SIGN, "Sign");
    addText(TXT_RADIO_DIRECTION_INPUT, "Input");
    addText(TXT_RADIO_DIRECTION_OUTPUT, "Output");

    addText(TXT_COLUMN_NAME, "Name");
    addText(TXT_COLUMN_COMPULSORY, "Compulsory");
    addText(TXT_COLUMN_VALUE, "Value");
    addText(TXT_COLUMN_TYPE, "Type");
    addText(TXT_COLUMN_WIDTH, "Width");
    addText(TXT_COLUMN_MAU, "MAU");
    addText(TXT_COLUMN_BIT_WIDTH, "Bit Width");
    addText(TXT_COLUMN_INVERTED, "Inv");
    addText(TXT_COLUMN_INDEX, "Index");
    addText(TXT_COLUMN_PORT, "Port");
    addText(TXT_COLUMN_SLOT, "Slot");
    addText(TXT_COLUMN_DESTINATION, "Destination");
    addText(TXT_COLUMN_OPERAND, "Operand");
    addText(TXT_COLUMN_TRIGGERS, "T");
    addText(TXT_COLUMN_COMMAND, "Command");
    addText(TXT_COLUMN_SHORTCUT, "Shortcut");
    addText(TXT_COLUMN_TOOLBAR_BUTTONS, "Toolbar Buttons");
    addText(TXT_COLUMN_AVAILABLE_COMMANDS, "Available Commands");
    addText(TXT_COLUMN_MIN_ADDRESS, "Min-Address");
    addText(TXT_COLUMN_MAX_ADDRESS, "Max-Address");
    addText(TXT_COLUMN_BUS, "Bus");
    addText(TXT_COLUMN_SEGMENT, "Segment");
    addText(TXT_COLUMN_SRP, "SRP");

    addText(TXT_BUTTON_UP, "&Up");
    addText(TXT_BUTTON_DOWN, "&Down");
    addText(TXT_BUTTON_SHIFT_LEFT, "<<");
    addText(TXT_BUTTON_SHIFT_RIGHT, ">>");
    addText(TXT_BUTTON_INSERT, "<- &Insert");
    addText(TXT_BUTTON_REMOVE, "&Remove ->");
    addText(TXT_BUTTON_ATTACH, "&Attach");
    addText(TXT_BUTTON_DETACH, "&Detach");

    addText(TXT_YES, "yes");
    addText(TXT_NO, "no");
    addText(TXT_ALL, "all");

    // Global control unit dialog
    addText(TXT_GCU_DIALOG_TITLE, "Global Control Unit");
    addText(TXT_GCU_OPERATIONS_BOX, "Operations:");
    addText(TXT_GCU_PORTS_BOX, "Ports:");
    addText(TXT_GCU_DELAY_SLOTS, "Delay Slots:");

    // Bus dialog
    addText(TXT_BUS_DIALOG_TITLE, "Bus");
    addText(TXT_BUS_BUS_BOX, "Bus:");
    addText(TXT_BUS_SEGMENTS_BOX, "Segments:");
    addText(TXT_BUS_SI_BOX, "Short Immediate:");
    addText(TXT_BUS_REGISTER_GUARD_BOX, "Register File Guards:");
    addText(TXT_BUS_PORT_GUARD_BOX, "Function Unit Port Guards:");

    // Register file dialog
    addText(TXT_RF_DIALOG_TITLE, "Register File");
    addText(TXT_RF_PORTS_BOX, "Ports:");
    addText(TXT_RF_TYPE_NORMAL, "normal");
    addText(TXT_RF_TYPE_VOLATILE, "volatile");
    addText(TXT_RF_TYPE_RESERVED, "reserved");

    // Register file port dialog
    addText(TXT_RF_PORT_DIALOG_TITLE, "Register File Port");

    // Special register port dialog
    addText(TXT_SR_PORT_DIALOG_TITLE, "Special Register Port");

    // Instruction templates dialog
    addText(TXT_TEMPLATES_DIALOG_TITLE, "Instruction Templates");
    addText(TXT_TEMPLATES_TEMPLATES_BOX, "Instruction Templates:");
    addText(TXT_TEMPLATES_SLOTS_BOX, "Template Slots:");

    // Instruction template slot dialog
    addText(TXT_TEMPLATE_SLOT_DIALOG_TITLE, "Instruction Template Slot");

    // Immediate slot dialog
    addText(TXT_IMMEDIATE_SLOT_DIALOG_TITLE, "Immediate Slots");

    // Immediate unit dialog
    addText(TXT_IU_DIALOG_TITLE, "Immediate Unit");
    addText(TXT_IU_PORTS_BOX, "Ports:");
    addText(TXT_IU_TEMPLATES_BOX, "Templates:");

    // Immediate unit port dialog
    addText(TXT_IU_PORT_DIALOG_TITLE, "Immediate Unit Port");

    // Operation dialog
    addText(TXT_OPERATION_DIALOG_TITLE, "Operation");
    addText(TXT_OPERATION_USAGE_BOX, "Operand usage:");
    addText(TXT_OPERATION_RESOURCES_BOX, "Pipeline Resources:");
    addText(TXT_OPERATION_OPERANDS_BOX, "Operands:");

    // Function unit dialog
    addText(TXT_FU_DIALOG_TITLE, "Function Unit");
    addText(TXT_FU_PORTS_BOX, "Ports:");
    addText(TXT_FU_OPERATIONS_BOX, "Operations:");

    // Function unit port dialog
    addText(TXT_FU_PORT_DIALOG_TITLE, "Function Unit Port");

    // Bridge dialog
    addText(TXT_BRIDGE_DIALOG_TITLE, "Bridge");

    // Function unit guard dialog
    addText(TXT_FU_GUARD_DIALOG_TITLE, "Function Unit Guard");

    // Function unit guard dialog
    addText(TXT_RF_GUARD_DIALOG_TITLE, "Register File Guard");

    // Options dialog
    addText(TXT_OPTIONS_DIALOG_TITLE, "Options");
    addText(TXT_OPTIONS_ICONS, "Icons");
    addText(TXT_OPTIONS_TEXT_ICONS, "text & Icons");
    addText(TXT_OPTIONS_TEXT, "Text");

    // Address Space dialog
    addText(TXT_ADDRESS_SPACE_DIALOG_TITLE, "Address Space");
    addText(TXT_ADDRESS_SPACE_MIN_ADDRESS, "Min-Address:");
    addText(TXT_ADDRESS_SPACE_MAX_ADDRESS, "Max-Address:");

    // Address Spaces dialog
    addText(TXT_ADDRESS_SPACES_DIALOG_TITLE, "Address Spaces");
    
    // Explorer plugin call dialog
    addText(TXT_CALL_EXPLORER_PLUGIN_TITLE, "Call Explorer Plugin");
    
    // Edit parameter title
    addText(TXT_EDIT_PARAMETER_TITLE, "Edit Parameter");

    // Socket dialog
    addText(TXT_SOCKET_DIALOG_TITLE, "Socket");
    addText(TXT_SOCKET_ATTACHED_BOX, "Attached Buses:");
    addText(TXT_SOCKET_DETACHED_BOX, "Detached Buses:");

    // About dialog
    addText(TXT_ABOUT_DIALOG_TITLE, "About ");

    // Error messages.
    addText(MSG_ERROR_ILLEGAL_NAME,
            "The component name is illegal.\n"
            "Component name must consist of a letter followed by\n"
            "any number of letters, numbers or characters _:");
    addText(MSG_ERROR_ONE_GCU, "Only one Global Control Unit can exist.");
    addText(MSG_ERROR_CANNOT_BRIDGE,
            "Bridge cannot be added since the machine\n"
            "description doesn't contain two buses\n"
            "which could be legally bridged.\n");
    addText(MSG_ERROR_CANNOT_RF_GUARD,
            "Register file guard can't be added, "
            "because there are no register files in the machine.");
    addText(MSG_ERROR_CANNOT_FU_GUARD,
            "Function unit port guard can't be added, "
            "because there are no function unit ports in the machine.");
    addText(MSG_ERROR_OPCODE_SET,
            "The current operation code setting port "
            "is read by an operation at the beginning "
            "of execution. The port cannot be set to "
            "non operation code setting until the "
            "operation is modified.");
    addText(MSG_ERROR_SAME_NAME,
            "The name '%1%' is already name of %2% in the %3%. "
            "Rename the %4%.");
    addText(MSG_ERROR_EMPTY_TEMPLATE,
            "The machine already contains an empty instruction template.\n"
            "Another empty template can't be added until the empty "
            "template is removed or a slot is added to it.");
    addText(MSG_ERROR_NO_SLOTS_AVAILABLE,
            "The instruction template already contains all possible slots.");
    addText(MSG_ERROR_NO_IUS_AVAILABLE,
            "The instruction template already contains a slot for each "
            "destination immediate unit.");
    addText(MSG_ERROR_BUS_IT_SLOT_WIDTH,
            "Instruction template '%1%' contains a slot for this bus with "
            "width of %2%. The width of the bus can't be less than that.");
    addText(MSG_ERROR_BRIDGE_NAMES,
            "Bridge and the opposite bridge can't have the same name.");
    addText(MSG_ERROR_NO_OPCODE_OPERAND,
            "The parent unit of the operation doesn't "
            "have an opcode setting port, or the opcode\n"
            "setting port is not bound to an operand.\n"
            "Resources can't be added to the pipeline.");
    addText(MSG_ERROR_PIPELINE_START,
            "Pipeline usage must start with resource use or\n"
            "operand read on cycle 0-1");
    addText(MSG_ERROR_CANNOT_REMOVE, "The %1% can't be removed.\n");
    addText(MSG_ERROR_CANNOT_ADD, "The %1% can't be added.\n");
    addText(MSG_ERROR_OPERAND_NUM_RESERVED,
            "Number '%1%' is already reserved for an operand.");
    addText(MSG_ERROR, "Error:\n");
    addText(MSG_ERROR_MAX_READS,
            "The max-reads element must contain a nonnegative integer \n"
            "number not greater than the number of ports that can write \n"
            "to a bus.\n");
    addText(MSG_ERROR_MAX_WRITES,
            "The max-writes element must contain a nonnegative integer number \n"
            "not greater than the number of ports that can write to a bus,\n"
            "minus one if only one bidirectional port can read from a bus.");
    addText(MSG_ERROR_GUARD_EXISTS,
            "Bus already has equal guard.");
    addText(MSG_ERROR_UNKNOWN_COMMAND_ID,
            "Unknown command ID in options:\n\n\t %1%");
    addText(MSG_ERROR_LOADING_FILE, "Error loading file '%1%':\n");

    addText(MSG_ERROR_NO_IMMEDIATE_UNITS,
            "There are no immediate units in the machine architecture.\n");
    addText(MSG_ERROR_ID_EXISTS,
            "Conflicting address space ID found! Changes to address space "
            "ID number list were not saved.\n");

    addText(MSG_WARN_RES_WITHOUT_USAGES, 
            "Resources without usages found.\n"
            "Removing resource: ");

    addText(MSG_CONFIRM_OPERAND_DELETION,
            "The operand '%1%' is in use. Do you want to clear the operand "
            "reads and writes in addition to deleting the port binding? "
            "Deleting the operand reads and writes will remove the operand "
            "completely.");


    // Components and subcomponents
    addText(COMP_MACHINE, "machine description");
    addText(COMP_A_SEGMENT, "a segment");
    addText(COMP_SEGMENT, "segment");
    addText(COMP_A_BUS, "a transport bus");
    addText(COMP_BUS, "transport bus");
    addText(COMP_AN_OPERATION, "an operation");
    addText(COMP_OPERATION, "operation");
    addText(COMP_BRIDGE, "bridge");
    addText(COMP_A_BRIDGE, "a bridge");
    addText(COMP_RESOURCE, "resource");
    addText(COMP_A_RESOURCE, "a resource");
    addText(COMP_FUNCTION_UNIT, "function unit");
    addText(COMP_A_FUNCTION_UNIT, "a function unit");
    addText(COMP_GCU, "control unit");
    addText(COMP_AN_IMMEDIATE_UNIT, "an immediate unit");
    addText(COMP_IMMEDIATE_UNIT, "immediate unit");
    addText(COMP_A_REGISTER_FILE, "a register file");
    addText(COMP_REGISTER_FILE, "register file");
    addText(COMP_A_PORT, "a port");
    addText(COMP_PORT, "port");
    addText(COMP_A_TEMPLATE, "a template");
    addText(COMP_TEMPLATE, "template");
    addText(COMP_RESOURCE_USE, "resource use");
    addText(COMP_OPERAND_READ, "operand read");
    addText(COMP_AN_ADDRESS_SPACE, "an address space");
    addText(COMP_ADDRESS_SPACE, "address space");
    addText(COMP_A_SOCKET, "a socket");
    addText(COMP_IMM_SLOT, "immediate slot");
    addText(COMP_AN_IMM_SLOT, "an immediate slot");

    // Component status templates for the statusline.
    addText(STATUS_FUNCTION_UNIT, "Function Unit:  name = %1%");
    addText(STATUS_REGISTER_FILE,
            "Register File:  name = %1%,  type = %2%,  size = %3%,  "
            "width = %4%");
    addText(STATUS_IMMEDIATE_UNIT,
            "Immediate Unit:  name = %1%,  size = %2%,  width = %3%,  "
            "extension = %4%");
    addText(STATUS_GCU, "Global Control Unit:  name = %1%");
    addText(STATUS_BUS,
            "Transport Bus:  name = %1%,  width = %2%,  "
            "immediate width = %3%,  immediate extension = %4%");
    addText(STATUS_SEGMENT, "Segment: name = %1%");
    addText(STATUS_SOCKET, "Socket:  name = %1%");
    addText(STATUS_BRIDGE, "Bridge:  name = %1%");
    addText(STATUS_PORT, "Port:  name = %1%");
    addText(STATUS_FU_PORT, "Port:  name = %1%,  width = %2%,  triggers = %3%");
}


/**
 * The Destructor.
 */
ProDeTextGenerator::~ProDeTextGenerator() {
}


/**
 * Returns the only instance of this class.
 */
ProDeTextGenerator*
ProDeTextGenerator::instance() {
    if (instance_ == NULL) {
        instance_ = new ProDeTextGenerator();
    }
    return instance_;
}


/**
 * Deletes the only instance of this class.
 */
void
ProDeTextGenerator::destroy() {
    if (instance_ != NULL) {
	delete instance_;
	instance_ = NULL;
    }
}
