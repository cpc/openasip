/**
 * @file EstimatorCmdLineOptions.hh
 *
 * Declaration of EstimatorCmdLineOptions.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ESTIMATOR_CMDLINE_OPTIONS_HH
#define TTA_ESTIMATOR_CMDLINE_OPTIONS_HH

#include <string>

#include "CmdLineOptions.hh"

/**
 * Command line option class for Simulator.
 */
class EstimatorCmdLineOptions : public CmdLineOptions {
public:
    EstimatorCmdLineOptions();
    virtual ~EstimatorCmdLineOptions();

    virtual void printVersion() const;
    virtual void printHelp() const;

    bool runOnlyEstimations() const;
    bool totalArea() const;
    bool longestPath() const;
    bool totalEnergy() const;

    std::string TPEF();
    std::string traceDB();
    
private:
    /// Copying not allowed.
    EstimatorCmdLineOptions(const EstimatorCmdLineOptions&);
    /// Assignment not allowed.
    EstimatorCmdLineOptions& operator=(const EstimatorCmdLineOptions&);
};

#endif
