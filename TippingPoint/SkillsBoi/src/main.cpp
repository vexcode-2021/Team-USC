#include "main.h"
#include <string>

using namespace pros;
using namespace std;

Controller controller(E_CONTROLLER_MASTER);

RobotParams this_robot;

int which_robot = 1;
int num_robots = 1;
string robot_names[] = {"SKL"};

void calibrate_robot_params() {
    init_robot();
}

void init_positions() {
    drive_right_1.set_current_limit(2500);        
    drive_right_2.set_current_limit(2500);
    drive_right_3.set_current_limit(2500);
    drive_right_4.set_current_limit(2500);
    drive_left_1.set_current_limit(2500);
    drive_left_2.set_current_limit(2500);
    drive_left_3.set_current_limit(2500);
    drive_left_4.set_current_limit(2500);
    back_lift_right.set_current_limit(2500);
    back_lift_left.set_current_limit(2500);
    front_lift_right.set_current_limit(2500);
    front_lift_left.set_current_limit(2500);
    side_lift.set_current_limit(2500);
    intake_right.set_current_limit(2500);
    intake_left.set_current_limit(2500);
    intake_wobble_left.set_current_limit(2500);
    intake_wobble_right.set_current_limit(2500);

    back_lift_right.move_voltage(5000);
    back_lift_left.move_voltage(5000);
    front_lift_right.move_voltage(3000);
    front_lift_left.move_voltage(3000);
    side_lift.move_voltage(3000);
    
    delay(1000);
    while (controller.get_digital(DIGITAL_UP)) {
        delay(10);
    }

    back_lift_right.tare_position();
    back_lift_left.tare_position();
    front_lift_right.tare_position();
    front_lift_left.tare_position();
    side_lift.tare_position();

    back_lift_right.move_voltage(0);
    back_lift_left.move_voltage(0);
    front_lift_right.move_voltage(0);
    front_lift_left.move_voltage(0);
    side_lift.move_voltage(0);
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    imu_sensor.reset();
    while (imu_sensor.is_calibrating()) {
        delay(10);
    }
    calibrate_robot_params();
    yeet_release.set_value(0);
    yeet_retract.set_value(0);
    init_tracking();
    init_positions();
    delay(200);
    pros::Task drive_task(run_drive, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Drive Task");
    pros::Task display_task(run_display, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Display Task");
    pros::Task base_lift_task(run_base_lifts, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Base Lift Task");
    pros::Task inatke_task(run_intake, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Intake Task");
    pros::Task current_control_task(run_current_control, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Current Control Task");   
    pros::Task yeet_task(run_yeet, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Yeet Task");
    ADIAnalogIn encoder_lift ( {{ 7 , 8 }});
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
    ADIAnalogIn encoder_lift ( {{ 7 , 8 }});
    back_latch.set_value(0);
    front_latch.set_value(0); 
    if (which_auton == 3) {
        tip_latch = false;
        back_tip.set_value(0);
    }
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
    ADIAnalogIn encoder_lift ( {{ 7 , 8 }});
    back_latch.set_value(0);
    front_latch.set_value(0);
}


void opcontrol() {
    yeet_state = 0;             // Retract the piston from yeet
    drive_mode = DM_USER;       // Stop any unfinished drive move

    bool just_toggled_limit = false;
    bool just_toggled_auton = false;
    while (true) {

        if (controller.get_digital(DIGITAL_X)) {
            ADIDigitalOut encoder_lift ( { 7 , 8 }, true);
            if (!just_toggled_limit) {
                limit_current = !limit_current;
            }
            just_toggled_limit = true;
        }
        else {
            just_toggled_limit = false;
        }

        if (controller.get_digital(DIGITAL_UP)) {
            limit_current = false;
            ADIDigitalOut encoder_lift ( { 7 , 8 }, true);
        }

        // Toggle between robots and autons
        if (controller.get_digital(DIGITAL_UP) && controller.get_digital(DIGITAL_X)) {
            if (!just_toggled_auton) {
                which_auton++;
                if (which_auton >= num_autons) {
                    which_auton = 0;
                    which_robot = (which_robot + 1) % num_robots;
                }
                calibrate_robot_params();
            }
            just_toggled_auton = true;
        }
        else {
            just_toggled_auton = false;
        }

        delay(20);
    }
}
