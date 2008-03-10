/**
 * @file SimulatorCmdLineOptions.hh
 *
 * Declaration of SimulatorCmdLineOptions.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIM_CMDLINE_OPTIONS_HH
#define TTA_SIM_CMDLINE_OPTIONS_HH

#include <string>

#include "CmdLineOptions.hh"

/**
 * Command line option class for Simulator.
 */
class SimulatorCmdLineOptions : public CmdLineOptions {
public:
    SimulatorCmdLineOptions();
    virtual ~SimulatorCmdLineOptions();

    virtual void printVersion() const;
    virtual void printHelp() const;

    bool debugMode();
    std::string scriptString();
    
    std::string machineFile();
    std::string programFile();
    bool fastSimulationEngine();
    
private:
    /// Copying not allowed.
    SimulatorCmdLineOptions(const SimulatorCmdLineOptions&);
    /// Assignment not allowed.
    SimulatorCmdLineOptions& operator=(const SimulatorCmdLineOptions&);

    bool optionGiven(std::string key);    
};

#endif
