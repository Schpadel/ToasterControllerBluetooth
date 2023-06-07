/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 *
 * At the moment we are using the default settings, but they can be canged using a BleGamepadConfig instance as parameter for the begin function.
 *
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_16
 * (16 buttons by default. Library can be configured to use up to 128)
 *
 * Possible DPAD/HAT switch position values are:
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * (or HAT_CENTERED, HAT_UP etc)
 *
 * bleGamepad.setAxes sets all axes at once. There are a few:
 * (x axis, y axis, z axis, rx axis, ry axis, rz axis, slider 1, slider 2)
 *
 * Library can also be configured to support up to 5 simulation controls
 * (rudder, throttle, accelerator, brake, steering), but they are not enabled by default.
 *
 * Library can also be configured to support different function buttons
 * (start, select, menu, home, back, volume increase, volume decrease, volume mute)
 * start and select are enabled by default
 */

#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad bleGamepad;

// Last state of the buttons
int lastButtonState[4] = {0, 0, 0, 0};
int buttonPins[4] = {GPIO_NUM_23, GPIO_NUM_19, GPIO_NUM_5, GPIO_NUM_18};
int potiPin = GPIO_NUM_34;

void setup()
{

    // Initialize Button Pins
    pinMode(GPIO_NUM_23, INPUT_PULLUP); // DREHEN links         X
    pinMode(GPIO_NUM_19, INPUT_PULLUP); // DREHEN rechts        B
    pinMode(GPIO_NUM_5, INPUT_PULLUP);  // Bewegung nach unten   A
    pinMode(GPIO_NUM_18, INPUT_PULLUP); // Bewegung nach oben   Y

    // Potentiometer
    //pinMode(potiPin, INPUT); // get analog data from potentiometer

    // Test old: 115200
    Serial.begin(9600);
    Serial.println("Starting BLE work!");
    bleGamepad.begin();

    // The default bleGamepad.begin() above enables 16 buttons, all axes, one hat, and no simulation controls or special buttons
}

void loop()
{
    if (bleGamepad.isConnected())
    {

        // Poti stuff
        int poti = analogRead(potiPin);
        int potiMapped = map(poti, 666, 863, 0, 1023);
        Serial.println((String) "raw value: " + poti + " mapped value: " + potiMapped);

        // Button stuff

        
        for (int index = 0; index < 4; index++)
        {
            int currentButtonState = !digitalRead(buttonPins[index]);
            if (currentButtonState != lastButtonState[index])
            {
                switch (index)
                {
                case 0: // Button 1
                    if (currentButtonState == 1)
                    {
                        bleGamepad.press(BUTTON_1);
                    }
                    else
                    {
                        bleGamepad.release(BUTTON_1);
                    }
                    break;
                case 1: // Button 2
                    if (currentButtonState == 1)
                    {
                        bleGamepad.press(BUTTON_2);
                    }
                    else
                    {
                        bleGamepad.release(BUTTON_2);
                    }
                    break;
                case 2: // Button 3
                    if (currentButtonState == 1)
                    {
                        bleGamepad.press(BUTTON_3);
                    }
                    else
                    {
                        bleGamepad.release(BUTTON_3);
                    }
                    break;
                case 3: // Button 4
                    if (currentButtonState == 1)
                    {
                        bleGamepad.press(BUTTON_4);
                    }
                    else
                    {
                        bleGamepad.release(BUTTON_4);
                    }
                    break;
                }
                lastButtonState[index] = currentButtonState;
            }
            
        }

        bleGamepad.setAxes(potiMapped, 0, 0, 0, 0, 0, 0, 0);

        // Gyro sensor

        delay(100);
    }
}