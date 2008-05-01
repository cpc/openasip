/**
 * @file SchedulerFrontend.cc
 *
 * Implementation of SchedulerFrontend class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @author Vladimír Guzma 2008 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>
#include <boost/format.hpp>
#include <boost/timer.hpp>

#include "SchedulerFrontend.hh"
#include "SchedulerCmdLineOptions.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"
#include "TPEFProgramFactory.hh"
#include "ADFSerializer.hh"
#include "ProgramWriter.hh"
#include "BinaryWriter.hh"
#include "TPEFWriter.hh"
#include "StartableSchedulerModule.hh"
#include "HelperSchedulerModule.hh"
#include "SchedulerConfigurationSerializer.hh"
#include "Conversion.hh"
#include "Application.hh"
#include "Environment.hh"
#include "NullMachine.hh"
#include "BasicMachineCheckSuite.hh"
#include "Algorithms/InterPassData.hh"
#include "MachineCheckResults.hh"
#include "FileSystem.hh"

using std::string;
using std::vector;

using namespace TPEF;
using namespace TTAProgram;
using namespace TTAMachine;

/**
 * Constructor.
 */
SchedulerFrontend::SchedulerFrontend() :
    pluginLoader_(&SchedulerPluginLoader::instance()) {
    // for easier debugging of uncaught exceptions
    Application::initialize();
}

/**
 * Destructor.
 */
SchedulerFrontend::~SchedulerFrontend() {
}

/**
 * Schedule the scheduling chain according to the given options.
 *
 * @param options Command line options.
 * @exception IOException If there's an error opening the source
 * program or the target ADF or the configuration file.
 * @exception DynamicLibraryException If an error occurs while loading
 * scheduler pass plugins.
 * @exception ModuleRunTimeError If an run-time error occurs in scheduler
 * module.
 * @exception IllegalMachine If the loaded machine cannot be scheduled for.
 */
void
SchedulerFrontend::schedule(SchedulerCmdLineOptions& options)
    throw (Exception) {

    const TTAMachine::Machine* target = NULL;
    const TPEF::Binary* tpefBin = NULL;
    const TTAProgram::Program* source = NULL;
    const SchedulingPlan* schedulingPlan = NULL;
    UniversalMachine universalMachine;
    string sourceProgram = options.argument(1);
    string targetADF = "";

    if (options.isTargetADFDefined()) {
        targetADF = options.targetADF();
    }

    // read target adf (if defined)

    if (targetADF != "") {
        ADFSerializer adfSerializer;
        adfSerializer.setSourceFile(targetADF);

        try {
            target = adfSerializer.readMachine();
        } catch (const Exception& e) {
            string method = "SchedulerFrontend::schedule()";
            string msg = e.errorMessage();
            IOException ioe(__FILE__, __LINE__, method, msg);
            ioe.setCause(e);
            throw ioe;
        }
    } else {
        // no target adf defined, keep target as NULL.
        // delete &NullMachine::instance() is an very evil thing to do.
    }

    // read source program
    try {
        BinaryStream sourceBin(sourceProgram);
        tpefBin = BinaryReader::readBinary(sourceBin);

        TPEFProgramFactory* progFactory = NULL;

        if (tpefBin->type() == Binary::FT_OBJSEQ ||
            tpefBin->type() == Binary::FT_PURESEQ ||
            tpefBin->type() == Binary::FT_LIBSEQ) {
            progFactory = new TPEFProgramFactory(*tpefBin, universalMachine);

        } else if (tpefBin->type() == Binary::FT_MIXED) {
            if (target == NULL) {
                string msg = "ADF file required to open source program that "
                    "contains partially scheduled code.";
                throw IOException(__FILE__, __LINE__, __func__, msg);
            } else {
                progFactory = new TPEFProgramFactory(
                    *tpefBin, *target, universalMachine);
            }

        } else if (tpefBin->type() == Binary::FT_PARALLEL) {
            if (target == NULL) {
                delete target;
                delete tpefBin;
                string msg = "ADF file required to open parallel TPEF program.";
                throw IOException(__FILE__, __LINE__, __func__, msg);
            } else {
                progFactory = new TPEFProgramFactory(*tpefBin, *target);
            }

        } else {
            delete target;
            delete tpefBin;
            string msg = "Illegal source program file type.";
            throw IOException(__FILE__, __LINE__, __func__, msg);
        }

        source = progFactory->build();
        delete progFactory;

    } catch (const UnreachableStream& e) {
        delete target;
        delete tpefBin;
        delete source;

        string method = "SchedulerFrontend::schedule()";
        string msg = "Source file '" + sourceProgram + "' not found.";
        throw IOException(__FILE__, __LINE__, method, msg);

    } catch (const Exception& e) {
        delete target;
        delete tpefBin;
        delete source;

        string method = "SchedulerFrontend::schedule()";
        string msg = "Error opening binary '" + sourceProgram + "': "
            + e.fileName() + ":" + Conversion::toString(e.lineNum()) + ":"
            + e.procedureName() + ":" + e.errorMessage();
        IOException ioe(__FILE__, __LINE__, method, msg);
        ioe.setCause(e);
        throw ioe;
    }

    // set up scheduling chain from the scheduler configuration file
    string confFile = options.configurationFile();
    if (options.isConfigurationFileDefined()) {
        confFile = options.configurationFile();
    } else {
        confFile = Environment::oldGccSchedulerConf();
    }

    if (options.isVerboseSwitchDefined()) {
        Application::setVerboseLevel(Application::VERBOSE_LEVEL_INCREASED);
    }
    try {

        schedulingPlan = SchedulingPlan::loadFromFile(confFile);
    } catch (const DynamicLibraryException& e) {
        delete target;
        delete tpefBin;
        delete source;

        throw e;
    } catch (const Exception& e) {
        delete target;
        delete tpefBin;
        delete source;

        string msg = e.errorMessage();
        throw IOException(__FILE__, __LINE__, __func__, msg);
    }

    string outputFile = options.argument(1) + ".scheduled.tpef";
    if (options.isOutputFileDefined()) {
        outputFile = options.outputFile();
    }

    if (!FileSystem::fileIsWritable(outputFile) &&
        !FileSystem::fileIsCreatable(outputFile)) {
        string msg=  "Output file '" + outputFile;
        msg +=  "' can not be open for writing.";
        delete target;
        delete tpefBin;
        delete source;
        delete schedulingPlan;
        Application::logStream() << msg << std::endl;
        throw IOException(__FILE__, __LINE__, __func__, msg);
    }

    TTAProgram::Program* scheduled = NULL;
    try {
        scheduled = schedule(*source, *target, *schedulingPlan);
    } catch (const Exception& e) {
        delete target;
        delete tpefBin;
        delete source;
        delete schedulingPlan;
        throw;
    }

    // write out scheduled program

    BinaryStream tpefOut(outputFile);

    // create new binary and write it in the output file
    ProgramWriter progWriter(*scheduled);
    Binary* newBin =  progWriter.createBinary();
    TPEFWriter::instance().writeBinary(tpefOut, newBin);

    delete newBin;
    delete target;
    delete tpefBin;
    delete source;
    delete schedulingPlan;
    delete scheduled;
}

/**
 * Schedules the given sequential program against the given target machine.
 *
 * @param source The source sequential program.
 * @param target The target machine.
 * @param schedulingPlan The scheduling plan (configuration of passes).
 * @return Returns the scheduled program.
 * @exception IOException if module needs target machine but it is not
 * defined. ModuleRunTimeError if an run-time error occurs in scheduler
 * module.
 */
TTAProgram::Program*
SchedulerFrontend::schedule(
    const TTAProgram::Program& source, const TTAMachine::Machine& target,
    const SchedulingPlan& schedulingPlan)
    throw (Exception) {

    // validate the loaded MOM, so we don't even try to schedule to
    // "broken" machines with design errors such as units that are
    // not connected, etc.
    BasicMachineCheckSuite checks;
    MachineCheckResults results;
    if (!checks.run(target, results)) {
        std::string errorMessage = "Cannot compile for the machine:\n";
        for (int i = 0; i < results.errorCount(); ++i) {
            MachineCheckResults::Error error = results.error(i);
            errorMessage +=
                (boost::format("%d: %s\n") % i % error.second).str();
        }
        throw IllegalMachine(__FILE__, __LINE__, __func__, errorMessage);
    }


    TTAProgram::Program* sourceCopy = source.copy();

    // prepare passes and helpers

    try {
        for (int i = 0; i < schedulingPlan.passCount(); i++) {
            prepareModule(schedulingPlan.pass(i), *sourceCopy, target);
            for (int j = 0; j < schedulingPlan.helperCount(i); j++) {
                prepareModule(schedulingPlan.helper(i, j), *sourceCopy, target);
            }
        }
    } catch (const Exception& e) {
        delete sourceCopy;
        sourceCopy = NULL;
        throw e;
    }

    // run passes
    InterPassData interPassData;
    for (int i = 0; i < schedulingPlan.passCount(); i++) {
        StartableSchedulerModule& pass = schedulingPlan.pass(i);
        try {
            if (!pass.isStartable()) {
                delete sourceCopy;
                string message =
                    "Internal error: Tried to start non-startable plugin "
                    "module.";
                throw Exception(__FILE__, __LINE__, __func__, message);
            }
            pass.setInterPassData(interPassData);
        if (Application::verboseLevel() >=
            Application::VERBOSE_LEVEL_INCREASED) {
            /// Prints out info about modules starting and their execution
            /// times
            Application::logStream() << "Starting module: "
                << pass.shortDescription() << std::endl;
            boost::timer timer;
            pass.start();
            long elapsed = static_cast<unsigned long>(timer.elapsed());
            Application::logStream() << "  Module finished in "
                << elapsed/60 << " minutes "
                << "and " << elapsed % 60 << " seconds" << std::endl;
        } else {
            pass.start();
        }

        } catch (const ObjectNotInitialized& e) {
            delete sourceCopy;
            string message =
                "Plugin not properly initialized: " + e.errorMessage() +
                " Error in pass configuration?";
            throw Exception(__FILE__, __LINE__, __func__, message);
        } catch (const WrongSubclass& e) {
            delete sourceCopy;
            string message =
                "Main module of a pass could not be started independently. "
                "Possible error in plugin implementation.";
            Exception newExcp(__FILE__, __LINE__, __func__, message);
            newExcp.setCause(e);
            throw newExcp;
        } catch (const ModuleRunTimeError& e) {
            delete sourceCopy;
            throw ModuleRunTimeError(
                e.fileName(), e.lineNum(), e.procedureName(), e.errorMessage());
        } catch (const Exception& e) {
            delete sourceCopy;
          /// @todo the pass name
            ModuleRunTimeError mre(
                __FILE__, __LINE__, __func__,
                "The pass number " + Conversion::toString(i) + " could not finish successfully.");
            mre.setCause(e);
            throw mre;
        } catch (...) {
            abortWithError("Unknown exception leaked from pass.start()");
        }
    }
    return sourceCopy;
}

/**
 * Prepares given scheduler module with domain object modules.
 *
 * @param module Scheduler module to be prepared.
 * @param source Source program to be set to the module.
 * @param target Target machine to be set to the module.
 * @exception IOException If module needs target machine but it is not
 * defined.
 */
void
SchedulerFrontend::prepareModule(
    BaseSchedulerModule& module, TTAProgram::Program& source,
    const TTAMachine::Machine& target)
    throw (IOException, Exception) {

    if (module.needsProgram()) {
        module.setProgram(source);
    }
    if (module.needsProgramRepresentation()) {
        string message =
            "Module requires ProgramRepresentation, which is not yet "
            "implemented!";
        throw Exception(__FILE__, __LINE__, __func__, message);
    }
    if (module.needsTarget()) {
        if (&target == NULL || &target == &NullMachine::instance()) {
            string method = "SchedulerFrontend::prepareModule()";
            string msg =
                "A defined scheduler module requires target "
                "architecture, but none was given.";
            // TODO: better exception?
            throw IOException(__FILE__, __LINE__, method, msg);
        } else {
            module.setTarget(target);
        }
    }
    if (module.needsPluginLoader()) {
        module.setPluginLoader(*pluginLoader_);
    }
}
