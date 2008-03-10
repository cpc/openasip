/**
 * @file MachineTestReportTextGenerator.hh
 *
 * Declaration of MachineTestReportTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_MOM_TEXT_GENERATOR_HH
#define TTA_MOM_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Text generator used to store texts returned by MachineTestReporter.
 */
class MachineTestReportTextGenerator : public Texts::TextGenerator {
public:
    MachineTestReportTextGenerator();
    virtual ~MachineTestReportTextGenerator();

    /// Ids for the generated texts.
    enum {
        TXT_SOCKET_SEGMENT_CONN_EXISTS = Texts::LAST__,
        TXT_SOCKET_SEGMENT_CONN_ILLEGAL_REG,
        TXT_SOCKET_SEGMENT_CONN_BOTH_DIRS_ARE_ILLEGAL,
        TXT_SOCKET_PORT_CONN_ILLEGAL_REG,
        TXT_SOCKET_PORT_CONN_EXISTS,
        TXT_SOCKET_PORT_CONN_RF_PORT_CONN_EXISTS,
        TXT_SOCKET_PORT_CONN_WRONG_SOCKET_DIR,
        TXT_SOCKET_PORT_CONN_MAX_CONN,
        TXT_BRIDGE_ILLEGAL_REG,
        TXT_BRIDGE_LOOP,
        TXT_BRIDGE_EXISTS,
        TXT_BRIDGE_BRANCHED_BUS,
        TXT_SOCKET_DIR_UNKNOWN,
        TXT_SOCKET_NO_CONN,
        TXT_SOCKET_SAME_DIR_CONN,
        TXT_SOCKET_FORBIDDEN_DIR
    };

private:
    // copying prohibited
    MachineTestReportTextGenerator(const MachineTestReportTextGenerator&);
    // assingment prohibited
    MachineTestReportTextGenerator& operator=(
        const MachineTestReportTextGenerator&);
};

#endif
