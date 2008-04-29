/**
 * @file SchedulerCmdLineOptions.hh
 *
 * Declaration of SchedulerCmdLineOptions class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SCHEDULER_CMD_LINE_OPTIONS_HH
#define TTA_SCHEDULER_CMD_LINE_OPTIONS_HH

#include <string>
#include "CmdLineOptions.hh"

/**
 * Command line options for the command line interface of the
 * Scheduler (schedule).
 */
class SchedulerCmdLineOptions : public CmdLineOptions {
public:
    SchedulerCmdLineOptions();
    virtual ~SchedulerCmdLineOptions();

    bool isTargetADFDefined() const;
    std::string targetADF() const;
    bool isConfigurationFileDefined() const;
    std::string configurationFile() const;
    bool isOutputFileDefined() const;
    std::string outputFile() const;
    bool isVerboseSwitchDefined() const;
    virtual void printVersion() const;

private:
    /// Copying forbidden.
    SchedulerCmdLineOptions(const SchedulerCmdLineOptions&);
    /// Assignment forbidden.
    SchedulerCmdLineOptions& operator=(const SchedulerCmdLineOptions&);

    /// Long name of the ADF file parameter.
    static const std::string ADF_PARAM_NAME;
    /// Long name of the configuration file parameter.
    static const std::string CONF_PARAM_NAME;
    /// Long name of the output file parameter.
    static const std::string OUTPUT_PARAM_NAME;
    /// Description of the command line usage of the Scheduler.
    static const std::string USAGE;
    /// Switch for verbose output listing scheduler modules
    static const std::string VERBOSE_SWITCH;
};

#endif
