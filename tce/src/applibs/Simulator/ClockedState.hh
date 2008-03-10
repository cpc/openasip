/**
 * @file ClockedState.hh
 *
 * Declaration of ClockedState class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CLOCKED_STATE_HH
#define TTA_CLOCKED_STATE_HH

/**
 * Interface implemented by state classes that need update their internal
 * state whenever elapsing of a processor clock cycle is simulated.
 */
class ClockedState {
public:
    ClockedState();
    virtual ~ClockedState();

    virtual void endClock() = 0;
    virtual void advanceClock() = 0;
    virtual bool isIdle();

private:
    /// Copying not allowed.
    ClockedState(const ClockedState&);
    /// Assignment not allowed.
    ClockedState& operator=(const ClockedState&);
};

#endif
