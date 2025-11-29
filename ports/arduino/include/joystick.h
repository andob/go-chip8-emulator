#ifndef JOYSTICK_H
#define JOYSTICK_H
#include <Arduino.h>

class JoystickCallbacks {
public:
    virtual ~JoystickCallbacks() = default;

    virtual void onButtonPressed() {}
    virtual void onButtonReleased() {}
    virtual void onUpPressed() {}
    virtual void onUpReleased() {}
    virtual void onDownPressed() {}
    virtual void onDownReleased() {}
    virtual void onLeftPressed() {}
    virtual void onLeftReleased() {}
    virtual void onRightPressed() {}
    virtual void onRightReleased() {}
};

class Joystick {
private:
    int buttonPin;
    int xAxisPin;
    int yAxisPin;
    int axisDebounceTime;

    const int minAnalogValue = 0;
    const int maxAnalogValue = 1023;
    const int verticalThreshold = 20;
    const int horizontalThreshold = 20;

    enum VerticalDirection { CenterVertical, Up, Down };
    VerticalDirection currentVerticalDirection = CenterVertical;
    VerticalDirection previousVerticalDirection = CenterVertical;
    VerticalDirection parseYValue(long yValue) const;
    void checkVertically(long yValue);

    enum HorizontalDirection { CenterHorizontal, Left, Right };
    HorizontalDirection currentHorizontalDirection = CenterHorizontal;
    HorizontalDirection previousHorizontalDirection = CenterHorizontal;
    HorizontalDirection parseXValue(long xValue) const;
    void checkHorizontally(long xValue);

    bool currentButtonState = HIGH;
    void checkButton();

    JoystickCallbacks* callbacks;

public:
    Joystick(const int buttonPin, const int xAxisPin, const int yAxisPin)
        : buttonPin(buttonPin), xAxisPin(xAxisPin), yAxisPin(yAxisPin) { setup(); }

    void setup() const;

    void check();

    void setCallbacks(JoystickCallbacks* newCallbacks);
};

#endif
