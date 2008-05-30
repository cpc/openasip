/**
 * @file SimValue.hh
 *
 * Declaration of SimValue class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pjaaskel@cs.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#ifndef TTA_SIM_VALUE_HH
#define TTA_SIM_VALUE_HH

#include "BaseType.hh"

//////////////////////////////////////////////////////////////////////////////
// SimValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Class that represents values in simulation.
 *
 * This class represents any data type that can be manipulated by operations
 * of the target architecture template, and provides the interface to access
 * the data in three predefined types.
 *
 */
class SimValue {
public:
    SimValue();
    explicit SimValue(int width);
    explicit SimValue(int value, int width);

    virtual ~SimValue();

    SimValue(const SimValue& source);

    int width() const;
    bool isActive() const;
    void setActive();
    //void setWidth(std::size_t bitWidth);
    void clearActive();

    SimValue& operator=(const SIntWord& source);
    SimValue& operator=(const UIntWord& source);
    SimValue& operator=(const FloatWord& source);
    SimValue& operator=(const DoubleWord& source);
    SimValue& operator=(const SimValue& source);

    const SimValue operator+(const SIntWord& rightHand);
    const SimValue operator+(const UIntWord& rightHand);
    const SimValue operator+(const FloatWord& rightHand);
    const SimValue operator+(const DoubleWord& rightHand);

    const SimValue operator-(const SIntWord& rightHand);
    const SimValue operator-(const UIntWord& rightHand);
    const SimValue operator-(const FloatWord& rightHand);
    const SimValue operator-(const DoubleWord& rightHand);

    const SimValue operator/(const SIntWord& rightHand);
    const SimValue operator/(const UIntWord& rightHand);
    const SimValue operator/(const FloatWord& rightHand);
    const SimValue operator/(const DoubleWord& rightHand);

    const SimValue operator*(const SIntWord& rightHand);
    const SimValue operator*(const UIntWord& rightHand);
    const SimValue operator*(const FloatWord& rightHand);
    const SimValue operator*(const DoubleWord& rightHand);

    int operator==(const SimValue& rightHand) const;
    int operator==(const SIntWord& rightHand) const;
    int operator==(const UIntWord& rightHand) const;
    int operator==(const FloatWord& rightHand) const;
    int operator==(const DoubleWord& rightHand) const;

    int intValue() const;
    unsigned int unsignedValue() const;

    SIntWord sIntWordValue() const;
    UIntWord uIntWordValue() const;
    DoubleWord doubleWordValue() const;
    FloatWord floatWordValue() const;

    /// These are public for fast access in the compiled simulation engine.
    union Value {
        UIntWord uIntWord;
        SIntWord sIntWord;
        FloatWord floatWord;
        DoubleWord doubleWord;
    };

    /// The value data.
    Value value_;

    /// The bitwidth of the value.
    int bitWidth_;

private:

    /// True if the value represents a valid data word, false otherwise.
    bool active_;

    /// Mask for masking extra bits when returning unsigned value.
    UIntWord mask_;
};

//////////////////////////////////////////////////////////////////////////////
// NullSimValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class that is used to represent a null SimValue.
 *
 * All methods cause program abort with an error log message.
 *
 */
class NullSimValue {
public:
    static SimValue& instance();

private:
    NullSimValue();

    static SimValue instance_;

};

#define SIMULATOR_MAX_INTWORD_BITWIDTH 32

#include "SimValue.icc"

#endif
