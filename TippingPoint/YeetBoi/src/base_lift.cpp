#include "main.h"

Motor base_lift(19, SPEED, 1);
Motor base_release(10, SPEED, 1);
Motor base_rotate(20, SPEED, 1);


int base_lift_state = -1;
double base_lift_pos = 0;
double base_lift_target = 0;
double base_release_pos = 0;
double base_release_target = 0;


void run_base_lift(void* params) {

    bool just_toggled = false;
    
    while (true) {

        double base_lift_speed = 0;
        double base_release_speed = 0;
        double base_rotate_speed = 0;
        double base_spin_offset = 0;

        base_lift_pos = base_lift.get_position();
        base_release_pos = base_release.get_position();

        bool next_state = false;

        if (optical_state == SPIN_RIGHT) {
            base_rotate_speed = 127;
            base_spin_offset = this_robot.BASE_SPIN_OFFSET;
        }
        if (optical_state == SPIN_LEFT) {
            base_rotate_speed = -127;
            base_spin_offset = this_robot.BASE_SPIN_OFFSET;
        }

        if (optical_state == FORCE_SPIN) {
            base_rotate_speed = 127;
            base_spin_offset = this_robot.BASE_SPIN_OFFSET;
        }
        // if we're looking for the sticker and haven't seen it yet
        if ((optical_state == LOOK_FOR_YELLOW) || (optical_state == LOOK_FOR_STICKER)){
            base_spin_offset = this_robot.BASE_SPIN_OFFSET;
            if (is_black) base_rotate_speed = 100; //needs to be a positive speed
            else base_rotate_speed = 127;
            if (optical_state == LOOK_FOR_YELLOW) {
                base_spin_offset = this_robot.BASE_SPIN_OFFSET;
            }
        }
        else if (optical_state == FUCK_GO_BACK){
            base_spin_offset = this_robot.BASE_SPIN_OFFSET;
            base_rotate_speed = -90;
        }
        if (controller.get_digital(DIGITAL_A)) {
            optical_state = DO_NOTHING;
            base_lift_state = 2;
            base_spin_offset = this_robot.BASE_SPIN_OFFSET;
            base_rotate_speed = -127;

            
        }
        if (controller.get_digital(DIGITAL_Y)) {
            optical_state = DO_NOTHING;
            base_lift_state = 2;
            base_spin_offset = this_robot.BASE_SPIN_OFFSET;
            base_rotate_speed = 127;

            
            
        }
        if (controller.get_digital(DIGITAL_R1)) {
            if (!just_toggled) {
                next_state = true;
            }
            just_toggled = true;
        }
        else {
            just_toggled = false;
        }

        // if (controller.get_digital(DIGITAL_RIGHT) && (which_auton == 4)) {
        //     base_lift_state = 1;    // Ready to grab
        // }
        // if (controller.get_digital(DIGITAL_LEFT) && (which_auton == 4)) {
        //     base_lift_state = 2;    // Hold base
        // }
        
        switch (base_lift_state) {
            case -1:  // Do nothing (abort state)
                if (next_state)
                    base_lift_state = 1;
                break;
            case 1:  // Ready to grab state
                base_lift_target = this_robot.BASE_LIFT_READY_POS;
                base_release_target = this_robot.BASE_RELEASE_READY_POS;
                if (next_state)
                    base_lift_state = 2;
                break;
            case 2:  // Hold base state
                // will subtract an offset to lower to the ground if the base is high && being spun
                base_lift_target = this_robot.BASE_LIFT_HOLD_POS - base_spin_offset;
                base_release_target = this_robot.BASE_RELEASE_HOLD_POS;
                if (next_state)
                    base_lift_state = 3;
                break;
            case 3:  // Drop base state
                base_lift_target = this_robot.BASE_LIFT_DROP_POS;
                base_release_target = this_robot.BASE_RELEASE_DROP_POS;
                if (millis() % 500 > 250) {
                    base_rotate_speed = 127;
                } else {
                    base_rotate_speed = -127;
                }
                if (next_state)
                    base_lift_state = 1;
                break;

            case 119:   // hold tight
                base_lift_target = 1;
                base_release_target = this_robot.BASE_RELEASE_HOLD_POS;
                if (next_state)
                    base_lift_state = 3;
                break;
            default:
                base_lift_state = -1;
        }

        // P-controller
        if (base_lift_target != -1) {
            base_lift_speed = (base_lift_target - base_lift_pos) * this_robot.BASE_LIFT_RATE;
        }
        if (base_release_target != -1) {
            base_release_speed = (base_release_target - base_release_pos) * this_robot.BASE_RELEASE_RATE;
        }

        // Abort button
        if (controller.get_digital(DIGITAL_UP)) {
            optical_state = DO_NOTHING;
            base_lift_target = -1;
            base_release_target = -1;
            base_lift_speed = 0;
            base_lift_state = 0;
        }

        base_lift.move_voltage((12000 * base_lift_speed) / 127);
        base_release.move_voltage((12000 * base_release_speed) / 127);
        base_rotate.move_voltage((12000 * base_rotate_speed) / 127);

        pros::delay(10);
    }
}
