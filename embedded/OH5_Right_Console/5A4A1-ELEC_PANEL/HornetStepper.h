#ifndef HORNET_STEPPER_H
#define HORNET_STEPPER_H

#include <Stepper.h>
#include <Arduino.h>
#include <stdlib.h>

/**
 * @brief Class to handle Hornet gauge stepper motor functionality
 * @details This class encapsulates all the logic for controlling a Hornet gauge
 *          stepper motor, including zeroing, startup testing, and needle positioning
 */
class HornetStepper {
private:
    // Stepper motor configuration
    int stepsPerRevolution;
    int zeroPosition;
    int maxPosition;
    int directionForward;
    
    // Current state
    int targetPosition;
    int currentPosition;
    
    // Stepper motor object
    Stepper stepper;
    
    // Pin assignments for the 4 coils
    int coil1Pin;
    int coil2Pin;
    int coil3Pin;
    int coil4Pin;

public:
    /**
     * @brief Constructor for HornetStepper
     * @param steps Steps per revolution for the stepper motor
     * @param zero Zero position offset from mechanical stop
     * @param max Maximum position offset from mechanical stop
     * @param dirForward Direction for forward movement (1 or -1)
     * @param coil1 Pin for coil 1
     * @param coil2 Pin for coil 2
     * @param coil3 Pin for coil 3
     * @param coil4 Pin for coil 4
     */
    HornetStepper(int steps, int zero, int max, int dirForward, 
                int coil1, int coil2, int coil3, int coil4) 
        : stepsPerRevolution(steps), zeroPosition(zero), maxPosition(max),
          directionForward(dirForward),
          targetPosition(zero), currentPosition(zero),
          stepper(steps, coil1, coil2, coil3, coil4),
          coil1Pin(coil1), coil2Pin(coil2), coil3Pin(coil3), coil4Pin(coil4) {
    }
    
    /**
     * @brief Function to zero the gauge
     * @note The gauge is zeroed by resetting the needle to the far counter-clockwise 
     *       position, then to the zero position. You may hear clicking sounds. This is 
     *       normal and not damaging the gauge at speed = 5.
     */
    void findZero() {
        stepper.setSpeed(3);
        stepper.step(stepsPerRevolution * (-directionForward));       // reset needle to mech stop
        stepper.step(zeroPosition * directionForward);                // move to position 0
        currentPosition = zeroPosition;
        targetPosition = zeroPosition;
        delay(250);
        stepper.setSpeed(3);
    }
    
    /**
     * @brief Function to test gauge calibration and logic
     * @note During the test, the gauge is moved all the way. The while() loop is 
     *       blocking the processor during the test, but we do this only once during 
     *       Arduino startup.
     */
    void startupTest() {
        delay(1000);
        stepper.setSpeed(50);
        int steps = (maxPosition - zeroPosition) / 3;
        for (int i = 3; i >= 0; i--) {
            targetPosition = zeroPosition + steps * i;  // Set to 3, 2, 1, 0
            while (abs(targetPosition - currentPosition) > 1) {
                setNeedle();
                delay(10);  // Small delay between steps
            }
            delay(1000);
        }
        stepper.setSpeed(5);
    }
    
    /**
     * @brief Function to move the needle towards target position
     * @note The needle is moved just one step. In the next loop() iterations, 
     *       the needle will be moved again, as long as a difference exists. 
     *       Not operating the needle all the way within one loop() iteration 
     *       avoids blocking the processor.
     */
    void setNeedle() {
        if (targetPosition == currentPosition) {
            return;
        }
        int difference = targetPosition - currentPosition;
        if (difference > 0) {
            stepper.step(directionForward);
            currentPosition++;
        } else if (difference < 0) {
            stepper.step(-directionForward);
            currentPosition--;
        }
    }
    
    /**
     * @brief Set the target position for the needle
     * @param target The target position to move to
     */
    void setTarget(int target) {
        targetPosition = target;
    }

    /**
     * @brief Set the target position for the needle as 0 to 65535 value
     * @param target The target position to move to
     */
    void setTarget(unsigned int target65535) {
        targetPosition = map(target65535, 0, 65535, 0, maxPosition);
    }
    
    /**
     * @brief Get the current target position
     * @return The current target position
     */
    int getTarget() const {
        return targetPosition;
    }
    
    /**
     * @brief Get the current needle position
     * @return The current needle position
     */
    int getCurrentPosition() const {
        return currentPosition;
    }
};

#endif // HORNET_STEPPER_H 