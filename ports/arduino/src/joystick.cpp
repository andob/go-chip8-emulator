#include "joystick.h"
#include <Arduino.h>

void Joystick::setup() const {
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(xAxisPin, INPUT);
    pinMode(yAxisPin, INPUT);
}

void Joystick::checkButton() {
    const bool previousButtonState = currentButtonState;
    currentButtonState = digitalRead(buttonPin);

    if (currentButtonState != previousButtonState && callbacks != nullptr) {
        if (previousButtonState == HIGH && currentButtonState == LOW) {
            callbacks->onButtonPressed();
        } else {
            callbacks->onButtonReleased();
        }
    }
}

Joystick::VerticalDirection Joystick::parseYValue(const long yValue) const {
    const long yPercent = map(yValue, minAnalogValue, maxAnalogValue, -100, 100);
    return yPercent < -verticalThreshold ? Up : yPercent > verticalThreshold ? Down : CenterVertical;
}

void Joystick::checkVertically(const long yValue) {
    previousVerticalDirection = currentVerticalDirection;
    currentVerticalDirection = parseYValue(yValue);

    if (currentVerticalDirection != previousVerticalDirection && callbacks != nullptr) {
        if (currentVerticalDirection == Up) {
            callbacks->onUpPressed();
        }
        else if (currentVerticalDirection == Down) {
            callbacks->onDownPressed();
        }
        else if (currentVerticalDirection == CenterVertical && previousVerticalDirection == Up) {
            callbacks->onUpReleased();
        }
        else if (currentVerticalDirection == CenterVertical && previousVerticalDirection == Down) {
            callbacks->onDownReleased();
        }
    }
}

Joystick::HorizontalDirection Joystick::parseXValue(const long xValue) const {
    const long xPercent = map(xValue, minAnalogValue, maxAnalogValue, -100, 100);
    return xPercent < -horizontalThreshold ? Right : xPercent > horizontalThreshold ? Left : CenterHorizontal;
}

void Joystick::checkHorizontally(const long xValue) {
    previousHorizontalDirection = currentHorizontalDirection;
    currentHorizontalDirection = parseXValue(xValue);

    if (currentHorizontalDirection != previousHorizontalDirection && callbacks != nullptr) {
        if (currentHorizontalDirection == Left) {
            callbacks->onLeftPressed();
        }
        else if (currentHorizontalDirection == Right) {
            callbacks->onRightPressed();
        }
        else if (currentHorizontalDirection == CenterHorizontal && previousHorizontalDirection == Left) {
            callbacks->onLeftReleased();
        }
        else if (currentHorizontalDirection == CenterHorizontal && previousHorizontalDirection == Right) {
            callbacks->onRightReleased();
        }
    }
}

void Joystick::check() {
    const long xValue = analogRead(xAxisPin);
    checkHorizontally(xValue);

    const long yValue = analogRead(yAxisPin);
    checkVertically(yValue);

    checkButton();
}

void Joystick::setCallbacks(JoystickCallbacks* newCallbacks) {
    this->callbacks = newCallbacks;
}
