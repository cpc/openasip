/**
 * @file ModelObserver.hh
 *
 * Declaration of ModelObserver class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 */

#ifndef TTA_MODEL_OBSERVER_HH
#define TTA_MODEL_OBSERVER_HH

/**
 * An interface for observers of the Model.
 */
class ModelObserver {
public:
    /**
     * Updates the observer when the Model changes.
     */
    virtual void update() = 0;

    virtual ~ModelObserver() {}
};

#endif
