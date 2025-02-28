# Auton Commands  
  
*32 Commands auto-generated from "src/auton.cpp"*  

---

## TURN:  
Robot will turn to face the specified angle  
*BLOCKING, has timeout*  
### Usage:
  > TURN, drive_turn_target, command_time_out,  
### Arguments: (2)
  * **drive_turn_target**:	Angle to turn to (degrees)
  * **command_time_out**:	Timout for command (seconds)  
  
## BRAKE:  
turn brakeing on for drive motors  
*non-blocking, NO TIMEOUT*  
### Usage:
  > BRAKE,   
  
## FACE:  
Robot will turn to face the specified point  
*BLOCKING, has timeout*  
### Usage:
  > FACE, drive_target_x, drive_target_y, command_time_out,  
### Arguments: (3)
  * **drive_target_x**:	X target (field coord in inches)
  * **drive_target_y**:	Y target (field coord in inches)
  * **command_time_out**:	Timout for command (seconds)  
  
## DRIVE:  
Robot will drive in a direction, at a speed, for a time  
*BLOCKING, has timeout*  
### Usage:
  > DRIVE, drive_speed_target, drive_turn_target, command_time_out,  
### Arguments: (3)
  * **drive_speed_target**:	Speed to drive (-127, 127)
  * **drive_turn_target**:	Direction to face (degrees)
  * **command_time_out**:	Timout for command (seconds)  
  
## DRIVEDIST:  
Robot will drive in a direction, at a speed, for a distance  
*BLOCKING, has timeout*  
### Usage:
  > DRIVEDIST, drive_speed_target, drive_turn_target, drive_distance_target, command_time_out,  
### Arguments: (4)
  * **drive_speed_target**:	Speed to drive (-127, 127)
  * **drive_turn_target**:	Direction to face (degrees)
  * **drive_distance_target**:	Distance to drive (inches)
  * **command_time_out**:	Timout for command (seconds)  
  
## LIFTPOS:  
Move the four-bar lift to a speficied height  
*non-blocking, NO TIMEOUT*  
### Usage:
  > LIFTPOS, lift_target,  
### Arguments: (1)
  * **lift_target**:	Height for the lift (encoder ticks)  
  
## WRISTPOS:  
Move the spike wrist to a specified angle  
*non-blocking, NO TIMEOUT*  
### Usage:
  > WRISTPOS, spike_wrist_target,  
### Arguments: (1)
  * **spike_wrist_target**:	Angle for the wrist (degrees rel. to floor)  
  
## ARMPOS:  
Move the spike arm to a specified position  
*non-blocking, NO TIMEOUT*  
### Usage:
  > ARMPOS, spike_arm_target,  
### Arguments: (1)
  * **spike_arm_target**:	Position for the arm (encoder ticks)  
  
## END:  
End of routine  
*BLOCKING, NO TIMEOUT*  
### Usage:
  > END,   
  
## PAUSE:  
Do nothhing for a time  
*BLOCKING, has timeout*  
### Usage:
  > PAUSE, command_time_out,  
### Arguments: (1)
  * **command_time_out**:	Timout for command (seconds)  
  
## WAIT:  
Do nothing until condition is met, eg. WAIT, LIFTABOVE, 1000, 5,  
*BLOCKING, has timeout*  
### Usage:
  > WAIT, wait_condition, wait_parameter, command_time_out,  
### Arguments: (3)
  * **wait_condition**:	What to wait for (keyword)
  * **wait_parameter**:	Value to wait until (number)
  * **command_time_out**:	Timout for command (seconds)  
  
## DROP:  
Drops rings from spike  
*non-blocking, has timeout*  
### Usage:
  > DROP, command_time_out,  
### Arguments: (1)
  * **command_time_out**:	Timout for command (seconds)  
  
## DEPLOY:  
Deploys the robot  
*non-blocking, NO TIMEOUT*  
### Usage:
  > DEPLOY,   
  
## CHILLYEET:  
Deactivates yeet release piston  
*non-blocking, NO TIMEOUT*  
### Usage:
  > CHILLYEET,   
  
## DRIVETO:  
Robot will drive to target position on field  
*BLOCKING, has timeout*  
### Usage:
  > DRIVETO, drive_speed_target, drive_target_x, drive_target_y, command_time_out,  
### Arguments: (4)
  * **drive_speed_target**:	How fast to drive (-127, 127)
  * **drive_target_x**:	X target (field coord in inches)
  * **drive_target_y**:	Y target (field coord in inches)
  * **command_time_out**:	Timout for command (seconds)  
  
## SETPOS:  
Sets the robot's position on the field  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SETPOS, x_pos, y_pos,  
### Arguments: (2)
  * **x_pos**:	X coord (field coord in inches)
  * **y_pos**:	Y coord (field coord in inches)  
  
## SETDIR:  
Sets the robot's direction on the field  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SETDIR, dir,  
### Arguments: (1)
  * **dir**:	Direction (degrees)  
  
## BELLYPOS:  
Moves bellygrab to desired position  
*non-blocking, NO TIMEOUT*  
### Usage:
  > BELLYPOS, belly_grab_state,  
### Arguments: (1)
  * **belly_grab_state**:	Position (keyword)  
  
## BASEPOS:  
Moves base lift-turner to desired position  
*non-blocking, NO TIMEOUT*  
### Usage:
  > BASEPOS, base_lift_state,  
### Arguments: (1)
  * **base_lift_state**:	Position (keyword)  
  
## READYSPIKE:  
Moves the spike to the ready position  
*non-blocking, NO TIMEOUT*  
### Usage:
  > READYSPIKE,   
  
## YEET:  
Hekkin yeeeeet  
*BLOCKING, has timeout*  
### Usage:
  > YEET, yeet_distance, command_time_out,  
### Arguments: (2)
  * **yeet_distance**:	ARG;
  * **command_time_out**:	Timout for command (seconds)  
### Warnings: (1)  
* Argument yeet_distance is missing a description  
  
## RETRACTYEET:  
Retracts the yeet wheel such that drive wheels are on the ground  
*non-blocking, NO TIMEOUT*  
### Usage:
  > RETRACTYEET,   
  
## COLLECTRING:  
Stamps down with the spike to pick up a ring  
*BLOCKING, has timeout*  
### Usage:
  > COLLECTRING, command_time_out,  
### Arguments: (1)
  * **command_time_out**:	Timout for command (seconds)  
  
## DEPOSITPOS:  
Moves the spike to the desired position for scoring  
*non-blocking, NO TIMEOUT*  
### Usage:
  > DEPOSITPOS, lift_state,  
### Arguments: (1)
  * **lift_state**:	Scoring position (keyword)  
  
## SPIKEBACKWARDSCORE:  
Moves the spike to the position for backwards scoring on a alliance goal  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SPIKEBACKWARDSCORE,   
  
## SPINOPTICAL:  
  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SPINOPTICAL,   
### Warnings: (1)  
* Command is missing description  
  
## SPINTIME:  
  
*BLOCKING, has timeout*  
### Usage:
  > SPINTIME, command_time_out,  
### Arguments: (1)
  * **command_time_out**:	Timout for command (seconds)  
### Warnings: (2)  
* Command is missing description  
* Command needs to print name: cout << "SPINTIME" << endl;  
  
## AUTOPARK:  
Autopark the robot  
*BLOCKING, NO TIMEOUT*  
### Usage:
  > AUTOPARK, drive_turn_target,  
### Arguments: (1)
  * **drive_turn_target**:	ARG;  
### Warnings: (1)  
* Argument drive_turn_target is missing a description  
  
## SETTILT:  
Set the tilt of the imu to 0  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SETTILT, imu_sensor.set_pitch(process_entry()),  
### Arguments: (1)
  * **imu_sensor.set_pitch(process_entry())**:	imu_sensor.set_pitch(ARG);  
### Warnings: (2)  
* Argument imu_sensor.set_pitch(process_entry()) is missing a description  
* Command needs to print name: cout << "SETTILT" << endl;  
  
## STOPSPIN:  
stop the goal spinning  
*non-blocking, NO TIMEOUT*  
### Usage:
  > STOPSPIN,   
### Warnings: (1)  
* Command needs to print name: cout << "STOPSPIN" << endl;  
  
## SPINGOALRIGHT:  
stop the goal spinning  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SPINGOALRIGHT,   
### Warnings: (1)  
* Command needs to print name: cout << "SPINGOALRIGHT" << endl;  
  
## SPINGOALLEFT:  
stop the goal spinning  
*non-blocking, NO TIMEOUT*  
### Usage:
  > SPINGOALLEFT,   
### Warnings: (1)  
* Command needs to print name: cout << "SPINGOALLEFT" << endl;  
  
