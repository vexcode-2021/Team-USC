////////////////////////////////////////////////
// LITTLE BOT CONFIG
//

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>

////////////////////////////////////////////////////////////////
// Anything that we will use in multiple files
// must be declared as extern in this file.
// Must only be initialised w/o extern in one file.
// Then all files can view and use it
//

using namespace pros;
using namespace std;

#define DM_USER -1      // Default user control
#define DM_YEET_FORWARD 1
#define DM_YEET_BACKWARD 2
#define DM_DISTANCE 3   // Drive straight for distance
#define DM_TIME 4       // Drive straight for time
#define DM_GOTO 5      // Drive to a location
#define DM_TURN 6       // Turn on spot to angle
#define DM_FACE 7       // Turn on spot to face point
#define DM_BRAKE 8      // Don't move
#define DM_AUTO_PARK 9      // Auto park
#define DM_FINAL_BALANCE 10      // Auto park stage 2

#define TURBO E_MOTOR_GEARSET_6
#define SPEED E_MOTOR_GEARSET_18
#define TORQUE E_MOTOR_GEARSET_36

#define READY 1
#define HOLD 2
#define PARK 5

extern double* auton_ptr[];
extern double mid_auton[];
extern double left_auton[];
extern double ring_practice_auton[];
extern double right_skills[];
extern double left_skills[];
extern double test_auton[];

extern int which_robot;
extern string robot_names[];

extern double auton_start_time;
extern int num_autons;
extern int which_auton;
extern string auton_names[];
extern double* auton_prt[];

// Main task prototypes
extern void run_drive(void* params);
extern void run_intake(void* params);
extern void run_display(void* params);
extern void run_base_lifts(void* params);
extern void run_yeet(void* params);
extern void run_current_control(void* params);


// Global functions - Position tracking
extern void track_position();
extern void set_direction(double dir);
extern double pythag(double x1, double y1, double x2, double y2);
extern void set_position(double x, double y, double d);
extern void set_position(double x, double y);
extern void init_tracking();

// Robot Config Functions
extern void init_robot();

// Controller
extern Controller controller;

extern IMU imu_sensor;

// Motors
extern Motor drive_right_1;
extern Motor drive_right_2;
extern Motor drive_right_3;
extern Motor drive_right_4;
extern Motor drive_left_1;
extern Motor drive_left_2;
extern Motor drive_left_3;
extern Motor drive_left_4;

extern Motor back_lift_right;
extern Motor back_lift_left;
extern Motor front_lift_right;
extern Motor front_lift_left;

extern Motor side_lift;

extern Motor intake_right;
extern Motor intake_left;
extern Motor intake_wobble_left;
extern Motor intake_wobble_right;


// Global variables
extern bool limit_current;
extern double average_temp;
extern double average_current;

extern double robot_theta;
extern double robot_x;
extern double robot_y;

extern int yeet_state;
extern double yeet_distance;

extern int drive_mode;

extern double drive_speed_target;
extern double drive_turn_target;
extern double drive_target_x;
extern double drive_target_y;
extern double drive_starting_x;
extern double drive_starting_y;
extern double drive_distance_target;
extern bool turn_correct;
extern double auto_park_min_power;
extern double max_tilt;

extern double last_auton_time;

extern int back_lift_state;
extern double back_lift_pos;
extern double back_lift_target;

extern int front_lift_state;
extern double front_lift_pos;
extern double front_lift_target;

extern double side_lift_pos;
extern double side_lift_target;

extern bool intake;
extern bool intake_wobble;
extern bool tip_latch;
extern bool front_latch_on;
extern bool back_latch_on;

// Sensors
extern ADIEncoder left_encoder;
extern ADIEncoder middle_encoder;
extern ADIEncoder right_encoder;

// Pistons
extern ADIDigitalOut yeet_release;
extern ADIDigitalOut yeet_retract;
extern ADIDigitalOut back_tip;
extern ADIDigitalOut back_latch;
extern ADIDigitalOut front_latch;
// extern ADIDigitalOut encoder_lift;

struct RobotParams {
    string name;
    double BASE_LIFT_YEET_POS;

    double INTAKE_VOMIT_THRESHOLD;
    double INTAKE_VOMIT_TIMEOUT;
    double INTAKE_VOMIT_TIME;

    double INTAKE_WOBBLE_IN;
    double INTAKE_WOBBLE_OUT;
    int INTAKE_WOBBLE_PERIOD_LEFT;
    int INTAKE_WOBBLE_PERIOD_RIGHT;

    double BACK_LIFT_READY_POS;
    double BACK_LIFT_HOLD_POS;
    double FRONT_WOBBLE_AMP;

    double FRONT_LIFT_READY_POS;
    double FRONT_LIFT_HOLD_POS;
    double BACK_LIFT_HEIGHT_DROP;
    double FRONT_LIFT_HEIGHT_DROP;
    double FRONT_LIFT_HEIGHT_DROP_HIGH;
    double FRONT_LIFT_PARK_POS;

    double SIDE_LIFT_READY_POS;
    double SIDE_LIFT_HOLD_POS;

    double BACK_LIFT_RATE;
    double FRONT_LIFT_RATE;
    double SIDE_LIFT_RATE;

    double LIFT_RATE;

    double SLEW_STEP_FORWARD;
    double SLEW_STEP_TURN;

    // How many inches of travel does the robot move per tick of the encoder
    double INCHES_PER_TICK;

    // How far apart are the tracking wheels
    double TRACKING_DIAMETER;

    // Fudge Factors
    double FORWARD_FUDGE_FACTOR;
    double STRAFE_FUDGE_FACTOR;
    double ROTATIONAL_FUDGE_FACTOR;

    // Auton drive stuff
    double DRIVE_TURN_RATE;
    double DRIVE_DRIVE_TURN_RATE;
    double DRIVE_RATE;
    double MIN_DRIVE_SPEED;
    double MAX_TURN_SPEED;
    double MIN_TURN_SPEED;
    int TURN_PULSE_ON;
    int TURN_PULSE_OFF;
    int TURN_PRECISION;
    double DRIVE_PRECISION;
    int TURN_CORRECT_TIME;
};

extern RobotParams this_robot;

#endif



