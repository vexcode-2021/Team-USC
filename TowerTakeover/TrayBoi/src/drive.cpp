////////////////////////////////////////////////
// TRAY BOT DRIVE
//

#include "main.h"

using namespace pros;
using namespace std;

// Drive dampening parameters (anti-tip)
#define DRIVE_DAMPEN 1
#define TURN_DAMPEN 1

// Joystick deadzone
#define DEADZONE 10

// Auton tuning parameters
#define TURN_RATE 0.5           // Bigger = slower
#define MAX_TURN_SPEED 80      // Bigger = faster
#define MAX_SLOW_TURN_SPEED 40      // Bigger = faster
#define MIN_TURN_SPEED 30       // Bigger = faster
#define MIN_DRIVE_SPEED 25      // Bigger = faster
#define DRIVE_DIST_RATE 16       // Bigger = slower

#define TURN_PULSE_ON 1         // Bigger = longer pulse
#define TURN_PULSE_OFF 16        // Bigger = longer wait between pulses

// Drive motor declarations
Motor driveR1(11,SPEED,1);
Motor driveR2(13,SPEED,0);
Motor driveR3(14,SPEED,1);
Motor driveR4(15,SPEED,1);

Motor driveL1(16,SPEED,0);
Motor driveL2(17,SPEED,0);
Motor driveL3(18,SPEED,1);
Motor driveL4(20,SPEED,0);

ADILineSensor whiteLineR(2);
ADILineSensor whiteLineL(3);

int driveMode = USER;
double faceDir = -1;
double driveSpeed = 0;
double targetDrivePos = 0;
bool slowTurn = false;

// Returns average encoder position of drive
double getDriveEncoderAvg() {
    double result = 0;
    
    result += driveL1.get_position();
    result += driveL2.get_position();
    result += driveL3.get_position();
    result += driveL4.get_position();
    
    result += driveR1.get_position();
    result += driveR2.get_position();
    result += driveR3.get_position();
    result += driveR4.get_position();
    
    result /= 8;
    
    return result;
}


// Drive task
void runDrive(void* params) {
    
    double turnSeek = -1;
    
    int turnPulseCounter = 0;
    
    double turnSpeed = 0;
    double forwardSpeed = 0;
    
    while (true) {
        
        pros::lcd::print(4, "Encoders: %f", getDriveEncoderAvg());
        
        pros::lcd::print(5, "Right: %d  Left: %d", whiteLineR.get_value(), whiteLineL.get_value());
        
        // User input
        double forwardSpeedUser = controller.get_analog(ANALOG_LEFT_Y);
        double turnSpeedUser = controller.get_analog(ANALOG_RIGHT_X);
        
        // Deadzone
        if (forwardSpeedUser * forwardSpeedUser < DEADZONE * DEADZONE)
            forwardSpeedUser = 0;
        if (turnSpeedUser * turnSpeedUser < DEADZONE * DEADZONE)
            turnSpeedUser = 0;
        
        
        turnSpeed = turnSpeed + (turnSpeedUser - turnSpeed) / TURN_DAMPEN;
        if (turnSpeed >= 0)
            turnSpeed = (turnSpeed*turnSpeed) / 127;
        else
            turnSpeed = -(turnSpeed*turnSpeed) / 127;
        
        forwardSpeed = forwardSpeed + (forwardSpeedUser - forwardSpeed) / DRIVE_DAMPEN;
        if (forwardSpeed >= 0)
            forwardSpeed = (forwardSpeed*forwardSpeed) / 127;
        else
            forwardSpeed = -(forwardSpeed*forwardSpeed) / 127;
        
        // Auton control code
        if (driveMode != USER) {
            
            // We have been given a direction to face, so record it
            turnSeek = faceDir;
            forwardSpeed = driveSpeed;
            
            double turnPower = direction - turnSeek;
            
            // Clamp turn to +/- 180 so we always turn shorter distance
            if (turnPower < 0) turnPower += 360;
            if (turnPower > 180) turnPower -= 360;
            
            // Decrease turn speed by tuning parameter
            turnPower /= TURN_RATE;
            
            // Clamp max speed
            if (slowTurn) {
                if (turnPower > MAX_SLOW_TURN_SPEED) turnPower = MAX_SLOW_TURN_SPEED;
                if (turnPower < -MAX_SLOW_TURN_SPEED) turnPower = -MAX_SLOW_TURN_SPEED;
            }
            else {
                if (turnPower > MAX_TURN_SPEED) turnPower = MAX_TURN_SPEED;
                if (turnPower < -MAX_TURN_SPEED) turnPower = -MAX_TURN_SPEED;
            }
            
            
            // Record sign
            bool negative = false;
            
            // Take abs value
            if (turnPower < 0) {
                turnPower *= -1;
                negative = true;
            }
            
            // If turn is small
            if (turnPower < MIN_TURN_SPEED) {
                
                // Incremnent pulse counter
                turnPulseCounter++;
                
                // If pulse is on, then set turnPower to min speed
                if (turnPulseCounter > TURN_PULSE_OFF) {
                    turnPower = MIN_TURN_SPEED;
                }
                
                // Wrap counter when limit reached
                if (turnPulseCounter > TURN_PULSE_ON + TURN_PULSE_OFF)
                    turnPulseCounter = 0;
                
            }
            
            // Undo the abs from earlier
            if (negative)
                turnPower *= -1;
            
            
            if (driveMode == DRIVE_DIST) {
                
                // Calculate slow down factor based on distance remaining
                double driveDistSlowDown = targetDrivePos - getDriveEncoderAvg();
               
                driveDistSlowDown /= DRIVE_DIST_RATE;
                
                // Make it positive
                if (driveDistSlowDown < 0) driveDistSlowDown *= -1;
                
                // Clamp to 1
                if (driveDistSlowDown > 1) driveDistSlowDown = 1;
                
                // Scale forwardSpeed by this factor
                forwardSpeed *= driveDistSlowDown;
            }
            
            // Make sure forward speed is greater that some min speed
            if (forwardSpeed > 0) {
                if (forwardSpeed < MIN_DRIVE_SPEED)
                    forwardSpeed = MIN_DRIVE_SPEED;
            }
            else if (forwardSpeed < 0) {
                if (forwardSpeed > -MIN_DRIVE_SPEED)
                    forwardSpeed = -MIN_DRIVE_SPEED;
            }
            
            if (faceDir == -1) {
                turnPower = 0;
            }
            
            // Finally set turn speed
            turnSpeed = turnPower;
        }
        
        
        // Cancel auto-function
        if (controller.get_digital(DIGITAL_UP)) {
            turnSeek = -1;
            faceDir = -1;
            forwardSpeed = 0;
            turnSpeed = 0;
            driveMode = USER;
        }
        
        
        // Finally set motor voltages
        driveR1.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR2.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR3.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR4.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveL1.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL2.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL3.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL4.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        
        
        // Let other tasks have a turn
        delay(10);
    }
}
