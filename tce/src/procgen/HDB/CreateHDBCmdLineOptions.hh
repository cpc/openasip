/**
 * @file CreateHDBCmdLineOptions.hh
 *
 * Declaration of CreateHDBCmdLineOptions.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CREATE_HDB_CMDLINE_OPTIONS_HH
#define TTA_CREATE_HDB_CMDLINE_OPTIONS_HH

#include <string>

#include "CmdLineOptions.hh"

/**
 * Command line option class for createhdb.
 */
class CreateHDBCmdLineOptions : public CmdLineOptions {
public:
    CreateHDBCmdLineOptions();
    virtual ~CreateHDBCmdLineOptions();

    virtual void printVersion() const;
    virtual void printHelp() const;

private:
    /// Copying not allowed.
    CreateHDBCmdLineOptions(const CreateHDBCmdLineOptions&);
    /// Assignment not allowed.
    CreateHDBCmdLineOptions& operator=(const CreateHDBCmdLineOptions&);
};

#endif
