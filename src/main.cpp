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
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BleGamepad.h>
#include <Wire.h>

BleGamepad bleGamepad("Gigachad Toaster", "Wir", 69);

// Button stuff
int lastButtonState[4] = {0, 0, 0, 0};
int buttonPins[4] = {GPIO_NUM_4, GPIO_NUM_19, GPIO_NUM_5, GPIO_NUM_18};
int potiPin = GPIO_NUM_34;
int potiMapped = 0;
int poti = 0;

// Gyro stuff
Adafruit_MPU6050 mpu;

void setup()
{

    // Initialize Button Pins
    pinMode(GPIO_NUM_4, INPUT_PULLUP); // DREHEN links         X
    pinMode(GPIO_NUM_19, INPUT_PULLUP); // DREHEN rechts        B
    pinMode(GPIO_NUM_5, INPUT_PULLUP);  // Bewegung nach unten   A
    pinMode(GPIO_NUM_18, INPUT_PULLUP); // Bewegung nach oben   Y

    // Potentiometer
    // pinMode(potiPin, INPUT); // get analog data from potentiometer

    // Test old: 115200
    Serial.begin(9600);

    // Start gyro stuff

    // Try to initialize!
    if (!mpu.begin())
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }
    Serial.println("MPU6050 Found!");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    Serial.print("Accelerometer range set to: ");
    switch (mpu.getAccelerometerRange())
    {
    case MPU6050_RANGE_2_G:
        Serial.println("+-2G");
        break;
    case MPU6050_RANGE_4_G:
        Serial.println("+-4G");
        break;
    case MPU6050_RANGE_8_G:
        Serial.println("+-8G");
        break;
    case MPU6050_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    Serial.print("Gyro range set to: ");
    switch (mpu.getGyroRange())
    {
    case MPU6050_RANGE_250_DEG:
        Serial.println("+- 250 deg/s");
        break;
    case MPU6050_RANGE_500_DEG:
        Serial.println("+- 500 deg/s");
        break;
    case MPU6050_RANGE_1000_DEG:
        Serial.println("+- 1000 deg/s");
        break;
    case MPU6050_RANGE_2000_DEG:
        Serial.println("+- 2000 deg/s");
        break;
    }

    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
    Serial.print("Filter bandwidth set to: ");
    switch (mpu.getFilterBandwidth())
    {
    case MPU6050_BAND_260_HZ:
        Serial.println("260 Hz");
        break;
    case MPU6050_BAND_184_HZ:
        Serial.println("184 Hz");
        break;
    case MPU6050_BAND_94_HZ:
        Serial.println("94 Hz");
        break;
    case MPU6050_BAND_44_HZ:
        Serial.println("44 Hz");
        break;
    case MPU6050_BAND_21_HZ:
        Serial.println("21 Hz");
        break;
    case MPU6050_BAND_10_HZ:
        Serial.println("10 Hz");
        break;
    case MPU6050_BAND_5_HZ:
        Serial.println("5 Hz");
        break;
    }

    Serial.println("");
    delay(100);

    // END gyro stuff

    Serial.println("Starting BLE work!");
    BleGamepadConfiguration bleGamepadConfig;
    bleGamepadConfig.setAxesMax(2000);
    bleGamepadConfig.setAxesMin(0);
    bleGamepadConfig.setAutoReport(true);
    bleGamepadConfig.setButtonCount(4);
    bleGamepadConfig.setWhichAxes(1,1,1,0,0,0,1,0);
    bleGamepad.begin(&bleGamepadConfig);

    // The default bleGamepad.begin() above enables 16 buttons, all axes, one hat, and no simulation controls or special buttons
}

void readGyroSensor()
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    
    /* Print out the values */
    /*
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");

    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degC");
    */

    //set joystick according to gyro data
    int xMapped = map(a.acceleration.x, -8, 8, 0, 2000);
    int yMapped = map(a.acceleration.y, -8, 8, 0, 2000);
    int zMapped = map(a.acceleration.z - 9.6, -8, 8, 0, 2000); // -9.6 adjust for gravity
    bleGamepad.setAxes(xMapped, yMapped, zMapped, 0, 0, 0, 0, 0);
    
}


void loop()
{
    if (bleGamepad.isConnected())
    {

        // Gyro stuff
        readGyroSensor();
        // Poti stuff
        poti = analogRead(potiPin);
        potiMapped = map(poti, 0, 4095, 0, 2000);
        bleGamepad.setSlider1(potiMapped);
        
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
    }
}