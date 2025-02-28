#include "main.h"
#include <string>

using namespace pros;
using namespace std;

void run_display(void* params) {
    int display_count = 0;
    screen::erase();
    controller.clear();
    while (true) {

        display_count++;

        // Main screen
        // screen::erase();
        // // Print position info to screen
        // screen::print(TEXT_MEDIUM, 1, "(%d, %d, %d)", left_encoder.get_value(), middle_encoder.get_value(), right_encoder.get_value());
        // screen::print(TEXT_MEDIUM, 2, "(%.2f, %.2f, %.2f)", robot_x, robot_y, robot_theta);

        // screen::print(TEXT_MEDIUM, 3, "Back Lift: %.2f %.2f", back_lift_pos, back_lift_target);
        // screen::print(TEXT_MEDIUM, 4, "Front Lift: %.2f %.2f", front_lift_pos, front_lift_target);
        // screen::print(TEXT_MEDIUM, 5, "Side Lift: %.2f %.2f", side_lift_pos, side_lift_target);
    
        // screen::print(TEXT_MEDIUM, 9, "Yeet: %d", yeet_state);

        // Controller display
        if (display_count % 3 == 0)
            controller.print(2, 0, "%.1f, %.1f, %.1f, %.1f                     ", robot_x, robot_y, robot_theta, imu_sensor.get_pitch());
            // controller.print(2, 0, "%d, %.3f                     ", (left_encoder.get_value() + right_encoder.get_value()), robot_theta);
        if (display_count % 3 == 1)
            controller.print(0, 0, "%s %s: %s           ", this_robot.name, robot_names[which_robot], auton_names[which_auton]);

        string meme = "Normal:";
        if (limit_current) {
            meme = "Boost:";
        }
        if (display_count % 3 == 2)
            controller.print(1, 0, "%s %.1f %.1f           ", meme, average_temp, average_current);

        delay(60);
    }
}
