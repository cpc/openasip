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
 * @file HDBToHtml.cc
 *
 * Implementation of HDBToHtml class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#include <time.h>

#include "HDBToHtml.hh"

#include "ContainerTools.hh"

#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FUImplementation.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "PipelineElement.hh"
#include "FUExternalPort.hh"
#include "RFExternalPort.hh"
#include "FUPortImplementation.hh"
#include "BlockImplementationFile.hh"
#include "RFEntry.hh"
#include "RFArchitecture.hh"
#include "RFImplementation.hh"
#include "RFPortImplementation.hh"
#include "HDBManager.hh"
#include "CostEstimationData.hh"
#include "DataObject.hh"
#include "CostFunctionPlugin.hh"

using namespace TTAMachine;
using namespace HDB;
using std::endl;


const std::string HDBToHtml::FU_ENTRIES = "FU Entries";
const std::string HDBToHtml::RF_ENTRIES = "RF Entries";
const std::string HDBToHtml::RF_IU_ENTRIES = "RF/IU Entries";
const std::string HDBToHtml::BUS_ENTRIES = "Bus Entries";
const std::string HDBToHtml::SOCKET_ENTRIES = "Socket Entries";
const std::string HDBToHtml::FU_ARCHITECTURES = "Function Units";
const std::string HDBToHtml::RF_ARCHITECTURES = "Register Files";
const std::string HDBToHtml::FU_IMPLEMENTATIONS = "FU Implementations";
const std::string HDBToHtml::RF_IMPLEMENTATIONS = "RF Implementations";
const std::string HDBToHtml::COST_PLUGINS = "Cost Function Plugins";
const std::string HDBToHtml::OPERATION_IMPLEMENTATIONS =
    "Operation Implementations";
const std::string HDBToHtml::OPERATION_IMPLEMENTATION_RESOURCES =
    "Operation Implementation Resources";

/**
 * The Cosntructor.
 *
 * @param hdb HDB 
 */
HDBToHtml::HDBToHtml(const HDB::HDBManager& hdb):
    hdb_(hdb) {
}

/**
 * The Destructor.
 */
HDBToHtml::~HDBToHtml() {
}

/**
 * Generates a html page of a function unit entry->
 *
 * @param id ID of the FU entry in the HDB.
 * @param stream Stream where the html is written to.
 */
void
HDBToHtml::fuEntryToHtml(RowID id, std::ostream& stream) {

    const HDB::FUEntry* entry = hdb_.fuByEntryID(id);

    stream << "<html><body><small>" << endl;
    stream << "<b>Function unit entry " << entry->id() << "</b><br>" << endl;

    stream << "<table>" << endl;
    stream << "<tr><td align=right><b>Architecture:</b></td><td>";
    if (entry->hasArchitecture()) {
        stream << "<a href=\"/" << FU_ARCHITECTURES << "/"
               << entry->architecture().id() << "\">"
               << entry->architecture().id() << "</a>"
               << "</td></tr>" << endl;
    } else {
        stream << "-</td></tr>";
    }

    stream << "<tr><td align=right><b>Implementation:</b></td><td>";
    if (entry->hasImplementation()) {
        stream << "<a href=\"/" << FU_IMPLEMENTATIONS << "/"
               << entry->implementation().id() << "\">"
               << entry->implementation().moduleName()
               << "</a></td></tr>" << endl;
    } else {
        stream << "-</td></tr>" << endl;
    }

    stream << "<tr><td align=right><b>Cost function plugin:</b></td><td>";
    if (entry->hasCostFunction()) {
        const CostFunctionPlugin& plugin = entry->costFunction();
        stream << "<a href=\"/" << COST_PLUGINS << "/"
               << plugin.id() << "\">"
               << plugin.name()
               << "</a></td></tr>" << endl;
    } else {
        stream << "-</td></tr>" << endl;
    }

    stream << "</table>";

    // Cost estimation data
    const std::set<RowID> costDataIDs = hdb_.fuCostEstimationDataIDs(id);
    if (!costDataIDs.empty()) {
        stream << "<b>Cost estimation data:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Value</th>"
               << "<th>Plugin</th></tr>" << endl;
    } else {
        stream << "No cost estimation data.<br>" << endl;
    }
    std::set<RowID>::const_iterator iter = costDataIDs.begin();
    for (; iter != costDataIDs.end(); iter++) {
        const CostEstimationData data = hdb_.costEstimationData(*iter);
        stream << "<tr><td>" << data.name() << "</td>"
               << "<td align=right>";
        stream << data.value().stringValue();
        stream << "</td><td align=center><a href=\"/" <<  COST_PLUGINS
               << "/" << data.pluginID() << "\">"
               << data.pluginID() << "</a></td></tr>" << endl;
    }
    stream << "</table>" << endl;
    stream << "</small></body></html>" << endl;

    delete entry;
}

/**
 * Generates a html page of a register file entry.
 *
 * @param id ID of the register file entry.
 * @param stream Stream where the html is written to.
 */
void
HDBToHtml::rfEntryToHtml(RowID id, std::ostream& stream) {

    const HDB::RFEntry* entry = hdb_.rfByEntryID(id);

    stream << "<html><body><small>" << endl;
    stream << "<b>Register file entry " << entry->id() << "</b><br>" << endl;

    stream << "<table>" << endl;
    stream << "<tr><td>Architecture:</td><td>";
    if (entry->hasArchitecture()) {
        stream << "<a href=\"/" << RF_ARCHITECTURES << "/"
               << entry->architecture().id() << "\">"
               << entry->architecture().id() << "</a>"
               << "</td></tr>" << endl;
    } else {
        stream << "-</td></tr>";
    }

    stream << "<tr><td><b>Implementation:</b></td><td>";
    if (entry->hasImplementation()) {
        stream << "<td><a href=\"/" << RF_IMPLEMENTATIONS << "/"
               << entry->implementation().id() << "\">"
               << entry->implementation().moduleName() << "</a>"
               << "</td></tr>" << endl;
    } else {
        stream << "-</td></tr>" << endl;
    }

    stream << "<tr><td align=right><b>Cost function plugin:</b></td><td>";
    if (entry->hasCostFunction()) {
        const CostFunctionPlugin& plugin = entry->costFunction();
        stream << "<a href=\"/" << COST_PLUGINS << "/"
               << plugin.id() << "\">"
               << plugin.name()
               << "</a></td></tr>" << endl;
    } else {
        stream << "-</td></tr>" << endl;
    }
    stream << "</table>";

    // Cost estimation data
    const std::set<RowID> costDataIDs = hdb_.rfCostEstimationDataIDs(id);
    if (!costDataIDs.empty()) {
        stream << "<b>Cost estimation data:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Value</th>"
               << "<th>Plugin</th></tr>" << endl;
    } else {
        stream << "No cost estimation data.<br>" << endl;
    }
    std::set<RowID>::const_iterator iter = costDataIDs.begin();
    for (; iter != costDataIDs.end(); iter++) {
        const CostEstimationData data = hdb_.costEstimationData(*iter);
        stream << "<tr><td>" << data.name() << "</td>"
               << "<td align=right>";
        stream << data.value().stringValue();
        stream << "</td><td align=center><a href=\"/" <<  COST_PLUGINS
               << "/" << data.pluginID() << "\">"
               << data.pluginID() << "</a></td></tr>" << endl;
    }
    stream << "</table>" << endl;
    stream << "</small></body></html>" << endl;
    delete entry;
}


/**
 * Generates a html page of a bus entry.
 *
 * @param id ID of the bus entry.
 * @param stream Stream where the html is written to.
 */
void
HDBToHtml::busEntryToHtml(RowID id, std::ostream& stream) {

    stream << "<html><body><small>" << endl;
    stream << "<b>Bus entry " << id << "</b><br>" << endl;

    stream << "<tr><td align=right><b>Cost estimation data:</b></td><td>";

    // Cost estimation data
    const std::set<RowID> costDataIDs = hdb_.busCostEstimationDataIDs(id);
    if (!costDataIDs.empty()) {
        stream << "<b>Cost estimation data:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Value</th>"
               << "<th>Plugin</th></tr>" << endl;
    } else {
        stream << "No cost estimation data.<br>" << endl;
    }
    std::set<RowID>::const_iterator iter = costDataIDs.begin();
    for (; iter != costDataIDs.end(); iter++) {
        const CostEstimationData data = hdb_.costEstimationData(*iter);
        stream << "<tr><td>" << data.name() << "</td>"
               << "<td align=right>";
        stream << data.value().stringValue();
        stream << "</td><td align=center><a href=\"/" <<  COST_PLUGINS
               << "/" << data.pluginID() << "\">"
               << data.pluginID() << "</a></td></tr>" << endl;
    }
    stream << "</table>" << endl;
    stream << "</small></body></html>" << endl;
}


/**
 * Generates a html page of a socket entry.
 *
 * @param id ID of the sovket entry.
 * @param stream Stream where the html is written to.
 */
void
HDBToHtml::socketEntryToHtml(RowID id, std::ostream& stream) {

    stream << "<html><body><small>" << endl;
    stream << "<b>Socket entry " << id << "</b><br>" << endl;

    stream << "<tr><td align=right><b>Cost estimation data:</b></td><td>";

    // Cost estimation data
    const std::set<RowID> costDataIDs = hdb_.socketCostEstimationDataIDs(id);
    if (!costDataIDs.empty()) {
        stream << "<b>Cost estimation data:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Value</th>"
               << "<th>Plugin</th></tr>" << endl;
    } else {
        stream << "No cost estimation data.<br>" << endl;
    }
    std::set<RowID>::const_iterator iter = costDataIDs.begin();
    for (; iter != costDataIDs.end(); iter++) {
        const CostEstimationData data = hdb_.costEstimationData(*iter);
        stream << "<tr><td>" << data.name() << "</td>"
               << "<td align=right>";
        stream << data.value().stringValue();
        stream << "</td><td align=center><a href=\"/" <<  COST_PLUGINS
               << "/" << data.pluginID() << "\">"
               << data.pluginID() << "</a></td></tr>" << endl;
    }
    stream << "</table>" << endl;
    stream << "</small></body></html>" << endl;
}


/**
 * Generates a html page of a function unit architecture.
 *
 * @param id ID of the FU architecture.
 * @param stream Stream where the html is written to.
 */
void
HDBToHtml::fuArchToHtml(RowID id, std::ostream& stream) {

    const FUArchitecture* arch = hdb_.fuArchitectureByID(id);
    const FunctionUnit& fu = arch->architecture();

    stream <<  "<html><body><small>" << endl;

    //
    //  PORTS
    // 
    stream << "<b>Ports:</b><br>" << endl;
    stream << "<table bgcolor=#bbbbbb>" << endl;
    stream << "<tr><th>Name</th><th>Width</th>"
           << "<th>Triggering</th>"
           << "<th>Opcode</th>"
           << "<th>Guard</th></tr>"
           << endl;

    for (int i = 0; i < fu.portCount(); i++) {

        const BaseFUPort& port = *fu.port(i);
        
        stream << "<tr><td align=left>" << port.name() << "</td>";

        if (arch->hasParameterizedWidth(port.name())) {
            stream << "<td align=right>param</td>";
        } else {
            stream << "<td align=right>" << port.width() << "</td>";
        }

        if (port.isTriggering()) {
            stream << "<td align=center><font color=#009900>yes</font></td>";
        } else {
            stream << "<td align=center><font color=#ff0000>no</font></td>";
        }

        if (port.isOpcodeSetting()) {
            stream << "<td align=center><font color=#009900>yes</font></td>";
        } else {
            stream << "<td align=center><font color=#ff0000>no</font></td>";
        }

        if (arch->hasGuardSupport(port.name())) {
            stream << "<td align=center><font color=#009900>yes</font></td>";
        } else {
            stream << "<td align=center><font color=#ff0000>no</font></td>";
        }
        stream << "</tr>" << endl;
    }
    stream << "</table><br><br>" << endl;    

    //
    // OPERATIONS
    //
    stream << "<b>Operations:</b><br><br>" << endl;
    for (int i = 0; i < fu.operationCount(); i++) {

        const HWOperation& operation = *fu.operation(i);
        const ExecutionPipeline& pipeline = *operation.pipeline();

        stream << "<b>" << operation.name() << "</b><br>" << endl;
        stream << "Latency: " << operation.latency() << endl;
        stream << "<br>" << endl;
        stream << "<table><tr><td valing=top>" << endl;
        stream << "<table bgcolor=#bbbbbb><tr><th>operand</th><th>port</th>";
        for (int cycle = 0; cycle < operation.latency(); cycle++) {
            stream << "<th width=20>" << cycle << "</th>";
        }
        stream << "</tr>" << endl;
        
        // Reads operands.
        ExecutionPipeline::OperandSet read = pipeline.readOperands();
        ExecutionPipeline::OperandSet::iterator iter = read.begin();
        for (; iter != read.end(); iter++) {
            stream << "<tr><td>" << *iter << "</td><td>"
                   << operation.port(*iter)->name() << "</td>";
            for (int cycle = 0; cycle < operation.latency(); cycle++) {
                if (ContainerTools::containsValue(
                        pipeline.readOperands(cycle), *iter)) {
                    stream << "<td align=center><b>R</b></td>";
                } else {
                    stream << "<td></td>";
                }
            }
            stream << "</tr>" << endl;
        }

        // Written operands.
        ExecutionPipeline::OperandSet written = pipeline.writtenOperands();
        iter = written.begin();
        for (; iter != written.end(); iter++) {
            stream << "<tr><td>" << *iter << "</td><td>"
                   << operation.port(*iter)->name() << "</td>";
            for (int cycle = 0; cycle < operation.latency(); cycle++) {
                if (ContainerTools::containsValue(
                        pipeline.writtenOperands(cycle), *iter)) {
                    stream << "<td align=center><b>W</b></td>";
                } else {
                    stream << "<td></td>";
                }
            }
            stream << "</tr>" << endl;
        }
        stream << "</table></td>" << endl;

        stream << "<td valign=top><table bgcolor=#bbbbbb>"
               << "<tr><th>resource</th>";
        for (int cycle = 0; cycle < operation.latency(); cycle++) {
            stream << "<th width=20>" << cycle << "</th>";
        }
        stream << "</tr>" << endl;

        // Resources used.
        for (int r = 0; r < fu.pipelineElementCount(); r++) {
            const PipelineElement& res = *fu.pipelineElement(r);
            stream << "<tr><td>" << res.name() << "</td>";
            for (int cycle = 0; cycle < operation.latency(); cycle++) {
                if (pipeline.isResourceUsed(res.name(), cycle)) {
                    stream << "<td align=center><b>X</b></td>";
                } else {
                    stream << "<td></td>";
                }
            }
            stream << "</tr>" << endl;
        }

        stream << "</table></td></tr></table><br><br>" << endl;
    }
    stream << "</small></body></html>" << endl;

    delete arch;
}


/**
 * Generates a html page of a register file architecture.
 *
 * @param id ID of the register file architecture.
 * @param stream Stream where the html is written to.
 */
void
HDBToHtml::rfArchToHtml(RowID id, std::ostream& stream) {

    const HDB::RFArchitecture* arch = hdb_.rfArchitectureByID(id);

    stream <<  "<html><body><small>" << endl;
    stream << "<table>" << endl;

    // Width
    stream << "<tr><td align=right><b>Width:</b></td>";
    if (arch->hasParameterizedWidth()) {
        stream << "<td>param</td>";
    } else {
        stream << "<td align=center>" << arch->width() << "</td>";
    }
    stream << "</tr>" << endl;

    // Size
    stream << "<tr><td align=right><b>Size:</b></td>";
    if (arch->hasParameterizedSize()) {
        stream << "<td align=center>param</td>";
    } else {
        stream << "<td align=center>" << arch->size() << "</td>";
    }
    stream << "</tr>" << endl;

    stream << "<tr><td align=right><b>Read ports:</b></td><td align=center>"
           << arch->readPortCount() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Write ports:</b></td><td align=center>"
           << arch->writePortCount() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Bidirectional ports:</b></td>"
           << "<td align=center>" << arch->bidirPortCount() << "</td></tr>"
           << endl;

    stream << "<tr><td align=right><b>Max reads:</b></td><td align=center>"
           << arch->maxReads() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Max Writes:</b></td><td align=center>"
           << arch->maxWrites() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Latency:</b></td><td align=center>"
           << arch->latency() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Guard support:</b></td>";
    if (arch->hasGuardSupport()) {
        stream << "<td align=center>yes</td></tr>";
        stream << "<tr><td align=right><b>Guard Latency:</b></td>"
	       << "<td align=center>" << arch->guardLatency()
	       << "</td></tr>" << endl;
    } else {
        stream << "<td align=center>no</td></tr>";
    }
    stream << "<tr><td align=right><b>Zero Register:</b></td><td" << 
    " align=center>" << arch->zeroRegister() << "</td></tr>" << endl;

    stream << "</table></small><br>" << endl;    
    stream << "</body></html>" << endl;

    delete arch;
}


/**
 * Generates a html page of a function unit implementation.
 *
 * @param id ID of the fu implementation.
 * @param stream Stream where the html is written to.
 */
void
HDBToHtml::fuImplToHtml(RowID id, std::ostream& stream) {

    RowID entryID = hdb_.fuEntryIDOfImplementation(id);
    const HDB::FUEntry* entry = hdb_.fuByEntryID(entryID);
    const HDB::FUImplementation& impl = entry->implementation();

    stream << "<html><body>" << endl;
    stream << "<small><table>" << endl;

    stream << "<tr><td align=right><b>Module name:</b></td><td align=left>"
           << impl.moduleName() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Opcode port:</b></td><td align=left>"
           << impl.opcodePort() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Clock port:</b></td><td align=left>"
           << impl.clkPort() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Reset port:</b></td><td align=left>"
           << impl.rstPort() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Global lock port:</b></td>"
           << "<td align=left>"
           << impl.glockPort() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Global lock req. port:</b></td>"
           << "<td align=left>"
           << impl.glockReqPort() << "</td></tr>" << endl;

    stream << "</table><br><br>" << endl;

    // Architecture ports.
    if (impl.architecturePortCount() > 0) {
        stream << "<b>Architecture ports:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th>"
               << "<th>Architecture port</th>"
               << "<th>Load port</th>"
               << "<th>Guard port</th>"
               << "<th>Width formula</th></tr>" << endl;
    } else {
        stream << "No architecture ports.<br>" << endl;
    }
    for (int i = 0; i < impl.architecturePortCount(); i++) {

        const HDB::FUPortImplementation& port = impl.architecturePort(i);
        stream << "<tr><td>" << port.name() << "</td>"
               << "<td>" << port.architecturePort() << "</td>"
               << "<td>" << port.loadPort() << "</td>"
               << "<td>" << port.guardPort() << "</td>"
               << "</td><td>" << port.widthFormula() << "</td></td>" << endl;
    }
    stream << "</table><br><br>" << endl;


    // Opcodes
    if (impl.opcodeCount() > 0) {
        stream << "<b>Opcodes:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Operation</th><th>Opcode</th></tr>" << endl;
    } else {
        stream << "No opcodes.<br>" << endl;
    }
    for (int i = 0; i < impl.opcodeCount(); i++) {
        stream << "<tr><td>" << impl.opcodeOperation(i) << "</td>"
               << "<td align=center>" << impl.opcode(impl.opcodeOperation(i))
               << "</td></tr>" << endl;
    }
    stream << "</table><br><br>" << endl;

    // External ports
    if (impl.externalPortCount() > 0) {
        stream << "<b>External ports:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Direction</th><th>Width formula</th>"
               << "<th>Parameter dependencies</th>"
               << "<th>Description</th></tr>" << endl;
    } else {
        stream << "No external ports.<br>" << endl;
    }
    for (int i = 0; i < impl.externalPortCount(); i++) {

        const HDB::FUExternalPort& port = impl.externalPort(i);
        stream << "<tr><td>" << port.name() << "</td><td align=center>";
        if (port.direction() == HDB::IN) {
            stream << "in";
        } else if (port.direction() == HDB::OUT) {
            stream << "out";
        } else if (port.direction() == HDB::BIDIR) {
            stream << "bidir";
        }
        stream << "</td><td>" << port.widthFormula() << "</td>";
        stream << "<td>";
        for (int dep = 0; dep < port.parameterDependencyCount(); dep++) {
            if (dep > 0) {
                stream << ", ";
            }
            stream << port.parameterDependency(dep);
        }
        stream << "</td><td>" << port.description() << "</td></tr>" << endl;
    }
    stream << "</table><br><br>" << endl;



    // Parameters
    if (impl.parameterCount() > 0) {
        stream << "<b>Parameters:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Type</th><th>Value</th></tr>" << endl;
    } else {
        stream << "No parameters.<br>" << endl;
    }
    for (int i = 0; i < impl.parameterCount(); i++) {

        const HDB::FUImplementation::Parameter& parameter = impl.parameter(i);
        stream << "<tr><td>" << parameter.name << "</td>"
               << "<td align=center>" << parameter.type << "</td>"
               << "<td>" << parameter.value << "</td></tr>" << endl;
    }
    stream << "</table><br><br>" << endl;


    // Implementation files
    if (impl.implementationFileCount() > 0) {
        stream << "<b>Implementation files:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Path</th><th>Format</th></tr>" << endl;
    } else {
        stream << "No implementation files.<br>" << endl;
    }
    for (int i = 0; i < impl.implementationFileCount(); i++) {
        const HDB::BlockImplementationFile& file = impl.file(i);
        stream << "<tr><td>" << file.pathToFile() << "</td>"
               << "<td align=center>";
        if (file.format() == HDB::BlockImplementationFile::VHDL) {
            stream << "VHDL";
        }else
        if (file.format() == HDB::BlockImplementationFile::Verilog) {
            stream << "Verilog";
        }

        stream << "</td></tr>" << endl;
    }
    stream << "</table><br><br>" << endl;


    // Cost estimation data
    const std::set<RowID> costDataIDs = hdb_.fuCostEstimationDataIDs(entryID);
    if (!costDataIDs.empty()) {
        stream << "<b>Cost estimation data:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Value</th>"
               << "<th>Plugin</th></tr>" << endl;
    } else {
        stream << "No cost estimation data.<br>" << endl;
    }
    std::set<RowID>::const_iterator iter = costDataIDs.begin();
    for (; iter != costDataIDs.end(); iter++) {
        const CostEstimationData data = hdb_.costEstimationData(*iter);
        stream << "<tr><td>" << data.name() << "</td>"
               << "<td align=right>";
        stream << data.value().stringValue();
        stream << "</td><td align=center><a href=\"/" <<  COST_PLUGINS
               << "/" << data.pluginID() << "\">"
               << data.pluginID() << "</a></td></tr>" << endl;
    }
    stream << "</table>" << endl;
    stream << "</small></body></html>" << endl;


    delete entry;
}

/**
 * Generates a html page of a register file implementation.
 *
 * @param id ID of the RF implementation.
 * @param stream Stream where the html is written to.
 */
void
HDBToHtml::rfImplToHtml(RowID id, std::ostream& stream) {

    RowID entryID = hdb_.rfEntryIDOfImplementation(id);
    const RFEntry* entry = hdb_.rfByEntryID(entryID);
    const RFImplementation& impl = entry->implementation();

    stream << "<html><body>" << endl;
    stream << "<small><table>" << endl;

    stream << "<tr><td align=right><b>Module name:</b></td><td align=center>"
           << impl.moduleName() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Clock port:</b></td><td align=center>"
           << impl.clkPort() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Reset port:</b></td><td align=center>"
           << impl.rstPort() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Global lock port:</b></td>"
           << "<td align=center>"
           << impl.glockPort() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Guard port:</b></td>"
           << "<td align=center>"
           << impl.guardPort() << "</td></tr>" << endl;

    stream << "<tr><td align=right><b>Size parameter:</b></td>"
           << "<td align=center>" << impl.sizeParameter() << "</td></tr>"
           << endl;

    stream << "<tr><td align=right><b>Width parameter:</b></td>"
           << "<td align=center>" << impl.widthParameter() << "</td></tr>"
           << endl;

    std::string sac_flag(impl.separateAddressCycleParameter() ?
        "true" : "false");
    stream << "<tr><td align=right><b>Separate address cycle:</b></td>"
           << "<td align=center>"
           << sac_flag << "</td></tr>" << endl;

    stream << "</table><br><br>" << endl;


    // Ports
    if (impl.portCount() > 0) {
        stream << "<b>Ports:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th>"
               << "<th>Direction</th>"
               << "<th>Load port</th>"
               << "<th>Opcode port</th>"
               << "<th>Opcode port width formula</th></tr>" << endl;
    } else {
        stream << "No ports.<br>" << endl;
    }
    for (int i = 0; i < impl.portCount(); i++) {

        const HDB::RFPortImplementation& port = impl.port(i);
        stream << "<tr><td>" << port.name() << "</td><td align=center>";
        if (port.direction() == HDB::IN) {
            stream << "in";
        } else if (port.direction() == HDB::OUT) {
            stream << "out";
        } else if (port.direction() == HDB::BIDIR) {
            stream << "bidir";
        }
        stream << "</td><td>" << port.loadPort() << "</td>"
               << "<td>" << port.opcodePort() << "</td>"
               << "</td><td>" << port.opcodePortWidthFormula()
               << "</td></td>" << endl;
    }
    stream << "</table><br><br>" << endl;

    // External ports
    if (impl.externalPortCount() > 0) {
        stream << "<b>External ports:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Direction</th><th>Width formula</th>"
            << "<th>Parameter dependencies</th>"
            << "<th>Description</th></tr>" << endl;
    } else {
        stream << "No external ports.<br>" << endl;
    }
    for (int i = 0; i < impl.externalPortCount(); i++) {

        const HDB::RFExternalPort& port = impl.externalPort(i);
        stream << "<tr><td>" << port.name() << "</td><td align=center>";
        if (port.direction() == HDB::IN) {
            stream << "in";
        } else if (port.direction() == HDB::OUT) {
            stream << "out";
        } else if (port.direction() == HDB::BIDIR) {
            stream << "bidir";
        }
        stream << "</td><td>" << port.widthFormula() << "</td>";
        stream << "<td>";
        for (int dep = 0; dep < port.parameterDependencyCount(); dep++) {
            if (dep > 0) {
                stream << ", ";
            }
            stream << port.parameterDependency(dep);
        }
        stream << "</td><td>" << port.description() << "</td></tr>" << endl;
    }
    stream << "</table><br><br>" << endl;

    // Parameters
    if (impl.parameterCount() > 0) {
        stream << "<b>Parameters:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Type</th><th>Value</th></tr>" << endl;
    } else {
        stream << "No parameters.<br>" << endl;
    }
    for (int i = 0; i < impl.parameterCount(); i++) {

        const HDB::RFImplementation::Parameter& parameter = impl.parameter(i);
        stream << "<tr><td>" << parameter.name << "</td>"
            << "<td align=center>" << parameter.type << "</td>"
            << "<td>" << parameter.value << "</td></tr>" << endl;
    }
    stream << "</table><br><br>" << endl;

    // Implementation files
    if (impl.implementationFileCount() > 0) {
        stream << "<b>Implementation files:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Path</th><th>Format</th></tr>" << endl;
    } else {
        stream << "No implementation files.<br>" << endl;
    }

    for (int i = 0; i < impl.implementationFileCount(); i++) {
        const HDB::BlockImplementationFile& file = impl.file(i);
        stream << "<tr><td>" << file.pathToFile() << "</td>"
               << "<td align=center>";
        if (file.format() == HDB::BlockImplementationFile::VHDL) {
            stream << "VHDL";
        }else
        if (file.format() == HDB::BlockImplementationFile::Verilog) {
            stream << "Verilog";
        }

        stream << "</td></tr>" << endl;
    }
    stream << "</table><br><br>" << endl;

    // Cost estimation data
    const std::set<RowID> costDataIDs = hdb_.rfCostEstimationDataIDs(entryID);
    if (!costDataIDs.empty()) {
        stream << "<b>Cost estimation data:</b>" << endl;
        stream << "<table bgcolor=#bbbbbb>" << endl;
        stream << "<tr><th>Name</th><th>Value</th>"
               << "<th>Plugin</th></tr>" << endl;
    } else {
        stream << "No cost estimation data.<br>" << endl;
    }
    std::set<RowID>::const_iterator iter = costDataIDs.begin();
    for (; iter != costDataIDs.end(); iter++) {
        const CostEstimationData data = hdb_.costEstimationData(*iter);
        stream << "<tr><td>" << data.name() << "</td>"
               << "<td align=right>";
        stream << data.value().stringValue();
        stream << "</td><td align=center><a href=\"/" << COST_PLUGINS
               << "/" << data.pluginID() << "\">"
               << data.pluginID() << "</a></td></tr>" << endl;
    }
    stream << "</table>" << endl;
    stream <<  "</small></body></html>" << endl;

    delete entry;
}

/**
 * Generates html page of a cost function plugin information.
 *
 * @param id Row id of the cost estimation plugin.
 * @param strem Stream where the html is written to.
 */
void
HDBToHtml::costFunctionPluginToHtml(RowID id, std::ostream& stream) {

    CostFunctionPlugin* plugin = hdb_.costFunctionPluginByID(id);

    stream << "<html><body><small>" << endl;
    stream << "<table bgcolor=#bbbbbb>"<< endl;

    // Plugin name.
    stream << "<tr><td align=right>Name:</td><td>" << plugin->name()
           << "</td></tr>" << endl;

    // Plugin type.
    stream << "<tr><td align=right>Type:</td><td>";
    if (plugin->type() == CostFunctionPlugin::COST_FU) {
        stream << "Function unit cost estimator.";
    } else if (plugin->type() == CostFunctionPlugin::COST_RF) {
        stream << "Register file cost estimator.";
    } else if (plugin->type() == CostFunctionPlugin::COST_DECOMP) {
        stream << "Decompressor cost estimator.";
    } else if (plugin->type() == CostFunctionPlugin::COST_ICDEC) {
        stream << "Interconnection network & decoder cost estimator.";
    } else {
        stream << "unknown";
    }
    stream << "</td></tr>" << endl;

    // Plugin file path.
    stream << "<tr><td align=right>File path:</td><td>"
           << plugin->pluginFilePath() << "</td></tr>" << endl;

    // Plugin description.
    stream << "<tr><td align=right>Description:</td><td>"
           << plugin->description() << "</td></tr>" << endl;

    stream << "</table><br><br>"<< endl;


    // Cost estiamtion data
    stream << "<b>Cost estimation data:</b>" << endl;
    stream << "<table bgcolor=#bbbbbb>"<< endl;
    stream << "<tr><th>Entry type</th><th>Entry ID</th>"
           << "<th>Key</th><th>Value</th></tr>" << endl;

    const std::set<RowID> dataIDs = hdb_.costFunctionPluginDataIDs(id);
    std::set<RowID>::const_iterator iter = dataIDs.begin();
    for (; iter != dataIDs.end(); iter++) {
        CostEstimationData data = hdb_.costEstimationData(*iter);
        stream << "<tr>";
        if (data.hasFUReference()) {
            // Function unit reference.
            stream << "<td>FU</td><td><a href=\"/" << FU_ENTRIES << "/"
                   << data.fuReference() << "\">"
                   << data.fuReference() << "</a>";
            stream << "</td>";
        } else if (data.hasRFReference()) {
            // Register file entry reference.
            stream << "<td>RF</td><td><a href=\"/" << RF_ENTRIES << "/"
                   << data.rfReference() << "\">"
                   << data.rfReference() << "</a>";
            stream << "</td>";
        } else if (data.hasBusReference()) {
            // Bus entry reference.
            stream << "<td>Bus</td><td>" 
                   << data.busReference() << "</a>";
            stream << "</td>";
        } else if (data.hasSocketReference()) {
            // Socket entry reference.
            stream << "<td>Socket</td><td>"
                   << data.socketReference() << "</a>";
            stream << "</td>";
        } else {
            stream << "<td>-</td><td>-</td>";
        }

        stream << "<td>" << data.name() << "</td><td>"
               << data.value().stringValue() << "</td></tr>" <<  endl;

    }
    stream << "</table>"<< endl;
    stream << "</small></body></html>" << endl;

    delete plugin;
}


void
HDBToHtml::OperationImplementationToHtml(RowID id, std::ostream& stream) {
    OperationImplementation op = hdb_.OperationImplementationByID(id);

    stream << "<html><body>";
    stream << "<b>Operation Implementation</b><br/>";
    stream << "operation : " << op.name << "<br/>";
    stream << "implementation (VHDL) : " << op.implFileVhdl << "<br/>";
    stream << "implementation (Verilog) : " << op.implFileVerilog << "<br/>";
    for(const auto& r : op.resources) {
        stream << "resource : " << r.name
               << " * " << r.count
               << "<br/>";
    }
    stream << "</body></html>\n";
}

void
HDBToHtml::OperationImplementationResourceToHtml(RowID id, std::ostream& stream) {
    OperationImplementationResource res
        = hdb_.OperationImplementationResourceByID(id);

    stream << "<html><body>";
    stream << "<b>Operation Implementation Resource</b><br/>";
    stream << "name : " << res.name << "<br/>";
    for (const auto& sim : res.simFiles) {
        stream << "simulation file : " << sim << "<br/>";
    }
    for (const auto& syn : res.synFiles) {
        stream << "synthesis file : " << syn << "<br/>";
    }
    stream << "</body></html>";
}
