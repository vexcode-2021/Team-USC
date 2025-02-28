#include "main.h"

#define USE_TANK_DRIVE false

using namespace pros;

Imu imu_sensor(6);

Motor drive_right_1(11, SPEED, 1);
Motor drive_right_2(12, SPEED, 0);
Motor drive_right_3(13, SPEED, 1);
Motor drive_right_4(16, SPEED, 1);

Motor drive_left_1(1, SPEED, 0);
Motor drive_left_2(5, SPEED, 1);
Motor drive_left_3(3, SPEED, 0);
Motor drive_left_4(4, SPEED, 0);

int drive_mode = DM_USER;

double drive_speed_target = 0;
double drive_turn_target = 0;
double drive_target_x = 0;
double drive_target_y = 0;
double drive_starting_x = 0;
double drive_starting_y = 0;
double drive_distance_target = 0;

double auto_park_min_power = 0;

bool turn_correct = false;
double max_tilt = 0;


void run_drive(void* params) {

    double drive_slew_forward = 0;
    double drive_slew_turn = 0;

    int turn_pulse_counter = 0;
    int counter = 0;
    int turn_correct_count = 0;

    double last_park_distance = 0;
    double last_tilt = 0;
    double time_tilting_down = 0;
    double time_not_tilting_down = 0;

    while (true) {
        counter++;

        // Call track_position to update the odometry
        // if (counter % 2)
        track_position();

        // Get user input for manual control of drive (arcade-style)
        double input_forward = controller.get_analog(ANALOG_LEFT_Y);
        double input_turn = controller.get_analog(ANALOG_RIGHT_X);

        // Quick fix to allow tank-style controls
        if (USE_TANK_DRIVE && (which_auton == 4)) {
            input_forward = (controller.get_analog(ANALOG_LEFT_Y) + controller.get_analog(ANALOG_RIGHT_Y)) / 2;
            input_turn = (controller.get_analog(ANALOG_LEFT_Y) - controller.get_analog(ANALOG_RIGHT_Y)) / 2;
        }

        if (drive_slew_forward > input_forward + this_robot.SLEW_STEP_FORWARD) {
            drive_slew_forward -= this_robot.SLEW_STEP_FORWARD;
        } 
        else if (drive_slew_forward < input_forward - this_robot.SLEW_STEP_FORWARD) {
            drive_slew_forward += this_robot.SLEW_STEP_FORWARD;
        }
        else {
            drive_slew_forward = input_forward;
        }

        if (drive_slew_turn > input_turn + this_robot.SLEW_STEP_TURN) {
            drive_slew_turn -= this_robot.SLEW_STEP_TURN;
        } 
        else if (drive_slew_turn < input_turn - this_robot.SLEW_STEP_TURN) {
            drive_slew_turn += this_robot.SLEW_STEP_TURN;
        }
        else {
            drive_slew_turn = input_turn;
        }
        

        double forward_speed = drive_slew_forward;
        double turn_speed = drive_slew_turn;

        if (controller.get_digital(DIGITAL_UP)) {
            drive_mode = DM_USER;
        }

        if (drive_mode != DM_USER) {
            if (drive_mode == DM_YEET_FORWARD) {
                turn_speed = 0;
                forward_speed = -127;
            }
            else if (drive_mode == DM_YEET_BACKWARD) {
                turn_speed = 0;
                forward_speed = 127;
            }
            // Otherwise we're doing an 'intelligent' drive (ie. aim and/or drive)
            else {

                ////////////////////////////////////
                // CALCULATIONS FOR TURNING
                //
                // For these modes we need to calculate angle to get us to the point
                if ((drive_mode == DM_FACE) || (drive_mode == DM_GOTO)) {
                    // Calculate angle to drive at
                    double delta_x = -drive_target_x + robot_x;
                    double delta_y = drive_target_y - robot_y;
                    
                    // Prevent div/0 error for next step
                    if (delta_x == 0)
                        delta_x = 0.0001;
                    
                    // Find angle to drive in
                    drive_turn_target = atan(delta_y / delta_x);
                    
                    // Convert to degrees
                    drive_turn_target = (180 * drive_turn_target) / (double)M_PI;

                    if (delta_x < 0)
                        drive_turn_target += 180;

                    drive_turn_target -= 90;

                    // Ok for driving backwards
                    if (drive_mode == DM_GOTO) {
                        if (drive_speed_target < 0) {
                            drive_turn_target += 180;
                        }
                    }

                    // Bound to [0,360]
                    while (drive_turn_target < 0) drive_turn_target += 360;
                    while (drive_turn_target > 360) drive_turn_target -= 360;
                    // if (!(counter % 50))
                    //     std::cout << "drive_turn_target: " << drive_turn_target << endl;

                    // If we are very close to target, stop turning
                    if (pythag(drive_target_x, drive_target_y, robot_x, robot_y) <= this_robot.DRIVE_PRECISION * 2) {
                        drive_turn_target = robot_theta;
                    }

                }

                ////////////////////////////////////
                // CALCULATIONS FOR SPEED
                //
                double forward_power = drive_speed_target;
                if ((drive_mode == DM_FACE) || (drive_mode == DM_TURN)) {
                    forward_power = 0;
                }
                else if (drive_mode == DM_DISTANCE) {
                    double remaining_dist = drive_distance_target - pythag(robot_x, robot_y, drive_starting_x, drive_starting_y);
                    forward_power = remaining_dist * this_robot.DRIVE_RATE * (drive_speed_target);
                    if (abs(forward_power) < this_robot.MIN_DRIVE_SPEED) {
                        if (forward_power > 0) {
                            forward_power = this_robot.MIN_DRIVE_SPEED;
                        }
                        else {
                            forward_power = -this_robot.MIN_DRIVE_SPEED;
                        }
                    }
                }
                else if (drive_mode == DM_GOTO) {
                    double remaining_dist = pythag(drive_target_x, drive_target_y, robot_x, robot_y);
                    forward_power =  remaining_dist * this_robot.DRIVE_RATE * drive_speed_target;
                    if (abs(forward_power) < this_robot.MIN_DRIVE_SPEED) {
                        if (forward_power > 0) {
                            forward_power = this_robot.MIN_DRIVE_SPEED;
                        }
                        else {
                            forward_power = -this_robot.MIN_DRIVE_SPEED;
                        }
                    }
                    if (remaining_dist <= this_robot.DRIVE_PRECISION) {
                        forward_power = 0;
                    }
                    double temp_error = drive_turn_target - robot_theta;
                    if (temp_error < 0) temp_error += 360;
                    if (temp_error > 180) temp_error -= 360;
                    if (abs(temp_error) > 20) {
                        forward_power = 0;
                    }
                }

                if (forward_power > abs(drive_speed_target)) forward_power = abs(drive_speed_target);
                if (forward_power < -abs(drive_speed_target)) forward_power = -abs(drive_speed_target);
                // Power becomes forward speed, later sent to the motors
                // Only set speed if we are driving somewhere, otheriwse, allow user control to do it
                if (forward_power != 0) {
                    forward_speed = forward_power;
                }
                //
                // END CALCULATIONS FOR SPEED
                ////////////////////////////////////


                // Calculate how far off we are from target location
                double turn_error = drive_turn_target - robot_theta;
                double turn_power = turn_error;

                if (abs(turn_error) <= this_robot.TURN_PRECISION) {
                    turn_power = 0;
                    turn_correct_count++;
                    if (turn_correct_count > this_robot.TURN_CORRECT_TIME) {
                        turn_correct = true;
                    }
                }
                else {
                    turn_correct_count = 0;
                }

                // Always turn least possible distance
                if (turn_power < 0) turn_power += 360;
                if (turn_power > 180) turn_power -= 360;

                // Adjust by tuning parameter
                if (abs(forward_power) > this_robot.MIN_DRIVE_SPEED) {  // Strictly greater than
                    turn_power *= this_robot.DRIVE_DRIVE_TURN_RATE;
                }
                else {
                    turn_power *= this_robot.DRIVE_TURN_RATE;
                }

                // Clamp to max speed
                if (turn_power > this_robot.MAX_TURN_SPEED) turn_power = this_robot.MAX_TURN_SPEED;
                if (turn_power < -this_robot.MAX_TURN_SPEED) turn_power = -this_robot.MAX_TURN_SPEED;

                // Take abs value & remember if we did
                bool negative = false;
                if (turn_power < 0) {
                    turn_power *= -1;
                    negative = true;
                }
                
                // If turn is small
                if ((turn_power < this_robot.MIN_TURN_SPEED) && (turn_power != 0)) {
                    // Increment pulse counter
                    turn_pulse_counter++;
                    // If pulse is on, then set turn_error to min speed
                    if (turn_pulse_counter > this_robot.TURN_PULSE_OFF) {
                        turn_power = this_robot.MIN_TURN_SPEED;
                    }
                    // Wrap counter when limit reached
                    if (turn_pulse_counter > this_robot.TURN_PULSE_ON + this_robot.TURN_PULSE_OFF) {
                        turn_pulse_counter = 0;
                    }
                }
                
                // Undo the abs from earlier
                if (negative) {
                    turn_power *= -1;
                }

                // Power becomes turn speed, later sent to the motors
                turn_speed = turn_power;
                //
                // END CALCULATIONS FOR TURNING
                ////////////////////////////////////

            }

            if (auto_park_min_power > 127) auto_park_min_power = 127;
            if (auto_park_min_power < -127) auto_park_min_power = -127;
            if (drive_mode == DM_AUTO_PARK) {      // AUTO PARK
                double tilt = -imu_sensor.get_pitch();
                double tot_displacement = pythag(robot_x, robot_y, drive_starting_x, drive_starting_y);
                double this_distance = tot_displacement - last_park_distance;
                last_park_distance = tot_displacement;

                if (this_distance <= 0.02) {
                    auto_park_min_power += 5;
                }
                else {
                    auto_park_min_power -= 1;
                }

                max_tilt = max(max_tilt, tilt);
                if ((tilt < max_tilt * 0.7)) {// && max_tilt > 20) { // 
                    drive_mode = DM_FINAL_BALANCE;
                    cout << ((millis() - auton_start_time) / 1000) << ": PARK TIPPING" << endl;
                    drive_starting_x = robot_x;
                    drive_starting_y = robot_y;
                    auto_park_min_power = 0;
                }

                if (tilt - last_tilt < -0.1) {
                    time_tilting_down++;
                    time_not_tilting_down = 0;
                }
                else if (tilt - last_tilt > -0.1) {
                    time_not_tilting_down++;
                }
                if (time_not_tilting_down > 0) {
                    time_tilting_down = 0;
                }

                forward_speed = auto_park_min_power;

                // cout << (forward_speed / 127.0) << "," << tot_displacement << "," << max_tilt * 0.75 << "," << tilt << "," << drive_mode-9 << endl;
                last_tilt = tilt;

            }


            if (drive_mode == DM_FINAL_BALANCE) {      // AUTO PARK
                double tilt = -imu_sensor.get_pitch();
                double tot_displacement = pythag(robot_x, robot_y, drive_starting_x, drive_starting_y);
                double this_distance = tot_displacement - last_park_distance;
                last_park_distance = tot_displacement;
                max_tilt = max(max_tilt, tilt);
                
                auto_park_min_power = -127;

                if (tot_displacement > 1.5) {//.5 
                    drive_mode = DM_BRAKE;
                    cout << ((millis() - auton_start_time) / 1000) << ": PARK DONE" << endl;
                }

                forward_speed = auto_park_min_power;

                // cout << (forward_speed / 127.0) << "," << tot_displacement << "," << max_tilt * 0.8 << "," << tilt << "," << drive_mode-9 << endl;

            }

        }

        if (drive_mode == DM_BRAKE) {
            forward_speed = 0;
            turn_speed = 0;

            drive_left_1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
            drive_left_2.set_brake_mode(E_MOTOR_BRAKE_HOLD);
            drive_left_3.set_brake_mode(E_MOTOR_BRAKE_HOLD);
            drive_left_4.set_brake_mode(E_MOTOR_BRAKE_HOLD);

            drive_right_1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
            drive_right_2.set_brake_mode(E_MOTOR_BRAKE_HOLD);
            drive_right_3.set_brake_mode(E_MOTOR_BRAKE_HOLD);
            drive_right_4.set_brake_mode(E_MOTOR_BRAKE_HOLD);            
        }
        else {
            drive_left_1.set_brake_mode(E_MOTOR_BRAKE_COAST);
            drive_left_2.set_brake_mode(E_MOTOR_BRAKE_COAST);
            drive_left_3.set_brake_mode(E_MOTOR_BRAKE_COAST);
            drive_left_4.set_brake_mode(E_MOTOR_BRAKE_COAST);

            drive_right_1.set_brake_mode(E_MOTOR_BRAKE_COAST);
            drive_right_2.set_brake_mode(E_MOTOR_BRAKE_COAST);
            drive_right_3.set_brake_mode(E_MOTOR_BRAKE_COAST);
            drive_right_4.set_brake_mode(E_MOTOR_BRAKE_COAST);
        }

        // Sent values to drive motors
        drive_right_1.move_voltage(12000 * (forward_speed - turn_speed) / 127);
        drive_right_2.move_voltage(12000 * (forward_speed - turn_speed) / 127);
        drive_right_3.move_voltage(12000 * (forward_speed - turn_speed) / 127);
        drive_right_4.move_voltage(12000 * (forward_speed - turn_speed) / 127);
        drive_left_1.move_voltage(12000 * (forward_speed + turn_speed) / 127);
        drive_left_2.move_voltage(12000 * (forward_speed + turn_speed) / 127);
        drive_left_3.move_voltage(12000 * (forward_speed + turn_speed) / 127);
        drive_left_4.move_voltage(12000 * (forward_speed + turn_speed) / 127);

        delay(10);
    }
}
