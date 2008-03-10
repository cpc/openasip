/**
 * @file ProDeClipboard.hh
 *
 * Declaration of ProDeClipboard class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_PRODE_CLIPBOARD_HH
#define TTA_PRODE_CLIPBOARD_HH

class ObjectState;

/**
 * A Clipboard class for copying machine components in ProDe.
 *
 * The clipboard can store one Component.
 * This class is implemented using the 'singleton' design pattern.
 */
class ProDeClipboard {
public:
    static ProDeClipboard* instance();
    static void destroy();
    void setContents(ObjectState* contents);
    ObjectState* copyContents();
    bool isEmpty();

private:
    ProDeClipboard();
    ~ProDeClipboard();

    static ProDeClipboard* instance_;
    ObjectState* contents_;
};
#endif
