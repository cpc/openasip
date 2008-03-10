/**
 * @file SocketEncoding.hh
 *
 * Declaration of SocketEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOCKET_ENCODING_HH
#define TTA_SOCKET_ENCODING_HH

#include "Encoding.hh"

class SocketCodeTable;
class SlotField;

/**
 * The SocketEncoding class models how socket sources or destinations are
 * mapped to control codes.
 *
 * Sockets have an independent encoding in each source or destination
 * field they appear in. A socket encoding can be more complex than a
 * fixed control code and encode many destinations or sources all
 * reached from the same socket. In this case, the socket encoding
 * contains SocketCodeTable instance.
 */
class SocketEncoding : public Encoding {
public:
    SocketEncoding(
	const std::string& name,
	unsigned int encoding,
	unsigned int extraBits,
	SlotField& parent)
	throw (ObjectAlreadyExists);
    SocketEncoding(const ObjectState* state, SlotField& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~SocketEncoding();

    SlotField* parent() const;

    std::string socketName() const;

    void setSocketCodes(SocketCodeTable& codeTable);
    void unsetSocketCodes();
    bool hasSocketCodes() const;
    SocketCodeTable& socketCodes() const;

    int socketIDPosition() const;
    int socketIDWidth() const;
    virtual int width() const;
    virtual int bitPosition() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for socket encoding.
    static const std::string OSNAME_SOCKET_ENCODING;
    /// ObjectState attribute key for the name of the socket.
    static const std::string OSKEY_SOCKET_NAME;
    /// ObjectState attribute key for the name of the socket code table.
    static const std::string OSKEY_SC_TABLE;

private:
    /// Name of the socket.
    std::string name_;
    /// Socket code table.
    SocketCodeTable* socketCodes_;
};

#endif
