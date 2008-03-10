/**
 * @file GlobalLock.hh
 *
 * Declaration of GlobalLock class.
 *
 * @author Jussi Nykänen 2005 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_GLOBAL_LOCK_HH
#define TTA_GLOBAL_LOCK_HH

/**
 * Represents a lock signal.
 */
class GlobalLock {
public:
    GlobalLock();
    virtual ~GlobalLock();

    void lock();
    void unlock();
    bool isLocked() const;

private:
    /// Copying not allowed.
    GlobalLock(const GlobalLock&);
    /// Assignment not allowed.
    GlobalLock& operator=(const GlobalLock&);

    /// Flag indicating whether lock signal is raised or not.
    bool isLocked_;
};

#endif
