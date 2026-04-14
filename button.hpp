#include <Arduino.h>

//#define DEBUG
// De-bounce time in milli-Seconds (default 50mS)
#define DEB_TIME 50

// State Callback
// First parameter current state and change Since
typedef void (*CB)(byte state, uint32_t since);

// One Button Menu Class
class OBM {

private:
    bool isPressed; // For Is Just Button Press Detected
    bool isDetected; // For Button Press was Debounced and Reported
    uint32_t ts; // Time Stamp of Event
    byte pin; // IO Pin Number
    uint8_t mode; // Mode For the Pin
    CB func; // Callback Function
    bool isConfigured; // Indicate that All configuration is complete
    uint32_t debounce_ms; // Debounce Registered duration

private:
    // Check if the Valid Button was initialized
    bool _valid()
    {
        if (isConfigured && pin != 255 && debounce_ms != 0 && mode != 255) {
            return true;
        }
        return false;
    }

private:
    // Check the things in Pressed State
    bool _checkInPressed(bool state, uint32_t ct)
    {
        // if the Required time has passed
        if (ts == 0 || ((ct - ts) < debounce_ms)) {
            return false;
        }

        // If the button is indeed in the Level Detected / Pressed state
        if (state) {
            isPressed = false;
            isDetected = true; // Set the Detected state
            uint32_t diff = ct - ts;
            ts = ct; // Set the Time when it was detected
#ifdef DEBUG
            Serial.println(F("Button Press Detected"));
#endif
            // Callback if available - Confirmed Press Event
            if (func != NULL) {
                func(true, diff);
            }

            // Return the Button Press Confirmed
            return true;
        }

        // Otherwise Button was released prematurely
        isPressed = false;
        isDetected = false; // Set the un-detected state
        ts = 0; // Clear the timeout
#ifdef DEBUG
        Serial.println(F("Button Released prematurely"));
#endif
        return false;
    }

private:
    // Check the things in Detected state
    bool _checkInDetected(bool state, uint32_t ct)
    {
        // Button is not Released - we wait
        if (state)
            return false;

        // Button was released
        isDetected = false; // Set the un-detected state
        isPressed = false;
        uint32_t diff = ct - ts;
        ts = 0; // Clear the timeout
#ifdef DEBUG
        Serial.println(F("Button Released Correctly"));
#endif
        // Callback if available - Released Event
        if (func != NULL) {
            func(false, diff);
        }
        // End of Released State Processing

        return false;
    }

public:
    // Blank Constructor for Default Cases
    OBM()
        : OBM(255, 255, NULL, 0)
    {
    }

    // Direct Specification type Constructor
    OBM(byte pin, uint8_t mode = INPUT_PULLUP)
        : OBM(pin, mode, NULL, 0)
    {
    }

    // Full Spec Constructor
    OBM(byte pn, uint8_t md, CB fn, uint32_t debounceTime_ms)
    {
        // Setup the Button
        isPressed = false;
        isDetected = false;
        ts = 0;
        func = fn;
        pin = pn;
        mode = md;
        isConfigured = false;
        debounce_ms = debounceTime_ms;
    }

    // Setup the Pins
    void setup(byte pin, uint8_t mode = INPUT_PULLUP)
    {
        pin = pin;
        mode = mode;
    }

    // Setup Call back function
    void attach(CB fn) { func = fn; }

    // Configure the Loop and Debounce
    bool begin(uint32_t debounceTime_ms = DEB_TIME)
    {
        if (pin == 255)
            return false;
        // Reset the Button Configuration
        pinMode(pin, mode);
        // Set the Debounce time
        if (debounce_ms < debounceTime_ms) {
            debounce_ms = debounceTime_ms;
        }
        // Set the Configuration is Done
        isConfigured = true;
        // Success
        return true;
    }

    // Loop Check For Single Press
    bool detect(PinStatus detlevel = LOW)
    {
        if (!_valid())
            return false;
        // Get the current time
        uint32_t ct = millis();
        // Read the button one more time
        bool state = (digitalRead(pin) == detlevel);

        // Check if the Button is in Pressed state
        if (isPressed) {
            return _checkInPressed(state, ct);
        }

        // if In detected State - Button was Pressed
        // We are waiting for its release
        if (isDetected) {
            return _checkInDetected(state, ct);
        }

        // If both above cases are not true and
        // Button was not touched earlier

        // Button got Just Pressed
        if (state) {
            // Button is indeed pressed
            isDetected = false;
            isPressed = true; // Pressed State for De-bounce
            ts = ct; // Timeout updated
#ifdef DEBUG
            Serial.println(F("Button Just Pressed"));
#endif
        }
        // Return the normally Non-Detected State
        return false;
    }
};
