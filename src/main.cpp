#include "main.h"

#include "autons.hpp"
#include "config.hpp"
#include "subsystem/lift.hpp"

/////
// For installation, upgrading, documentations, and tutorials, check out our website!
// https://ez-robotics.github.io/EZ-Template/
/////

void ez_screen_task();
pros::Task* ezScreenTask = nullptr;
pros::Task* liftTask = nullptr;
pros::Task* intakeTask = nullptr;

// ===========================================================================
// Last season (Push Back) helpers - reference only, nothing calls these.
// They drive the PB_ devices in config.cpp, several of which sit on ports the
// 2026-27 robot uses.  Check the ports before wiring any of it back up.
// ===========================================================================

// runtime variables
double forwards;
double turning;
float up;
float down;
bool clamped = false;
bool lifted = false;
int autoSelector = 0;
int separation_state = 0;

void arcadeCurve(pros::controller_analog_e_t power, pros::controller_analog_e_t turn, pros::Controller mast, float f) {
  up = mast.get_analog(power);
  down = mast.get_analog(turn);
  forwards = (exp(-f / 10) + exp((fabs(up) - 127) / 10) * (1 - exp(-f / 10))) * up;
  turning = -1 * down;

  leftMotors.move(forwards * 0.95 - turning);
  rightMotors.move(forwards * 0.95 + turning);
}

void printTelemetry() {
  while (true) {
    // DISABLED: Color sorting telemetry
    // switch (intake.ball) {
    //   case Intake::Ball::BLUE: controller.print(1, 1, "%s", "SEPARATING BLUE"); break;
    //   case Intake::Ball::RED: controller.print(1, 1, "%s", "SEPARATING RED"); break;
    //   case Intake::Ball::NONE: controller.print(1, 1, "%s", "SEPARATING NONE"); break;
    //   default: break;
    // }
    pros::delay(100);
  }
}

// DISABLED: Color sorting button control
// void switchSeparation() {
//     if (controller.get_digital(DIGITAL_UP)) {
//         separation_state++;
//         if (separation_state > 2) { separation_state = 0; }
//         switch (separation_state) {
//             case 0: intake.setSeparation(Intake::Ball::RED); break;
//             case 1: intake.setSeparation(Intake::Ball::BLUE); break;
//             case 2: intake.setSeparation(Intake::Ball::NONE); break;
//         }
//         pros::delay(500);
//     }
// }

static bool piston_state = false;
void intake_piston_toggle() {
  while (true) {
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
      piston_state = !piston_state;
      intakePiston.set_value(piston_state);
      pros::delay(300);
    }
  }
}

static bool flapper_state = false;
void flapper_piston_toggle() {
  while (true) {
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y)) {
      flapper_state = !flapper_state;
      flapperPiston.set_value(flapper_state);
      pros::delay(300);
    }
  }
}

// ===========================================================================
// End of last season's helpers
// ===========================================================================

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  // Print our branding over your terminal :D
  ez::ez_template_print();

  pros::delay(500);  // Stop the user from doing anything while legacy ports configure

  // Look at your horizontal tracking wheel and decide if it's in front of the midline of your robot or behind it
  //  - change `back` to `front` if the tracking wheel is in front of the midline
  //  - ignore this if you aren't using a horizontal tracker
  chassis.odom_tracker_back_set(&horiz_tracker);
  // Look at your vertical tracking wheel and decide if it's to the left or right of the center of the robot
  //  - change `left` to `right` if the tracking wheel is to the right of the centerline
  //  - ignore this if you aren't using a vertical tracker
  // chassis.odom_tracker_left_set(&vert_tracker);

  // Configure your chassis controls
  // Curve buttons are OFF: EZ's defaults for them are LEFT/RIGHT and Y/A, and A sends
  // the lift to MATCHLOADING.  With this enabled, every press of A would also nudge the turn
  // curve.  Tune the curve here instead and reflash.
  chassis.opcontrol_curve_buttons_toggle(false);
  chassis.opcontrol_drive_activebrake_set(0.0);   // Sets the active brake kP. We recommend ~2.  0 will disable.
  chassis.opcontrol_curve_default_set(0.0, 0.0);  // Throttle curve, turn curve.  0.0 is linear. (Comment this line out if you have an SD card!)

  // Set the drive to your own constants from autons.cpp!
  default_constants();

  // If you ever free up four buttons and want live curve tuning back, re-enable
  // opcontrol_curve_buttons_toggle above and remap them onto the free buttons here:
  // chassis.opcontrol_curve_buttons_left_set(pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT);  // If using tank, only the left side is used.
  // chassis.opcontrol_curve_buttons_right_set(pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU
  ez::as::auton_selector.autons_add({
      {"Drive\n\nDrive forward and come back", drive_example},
      {"Turn\n\nTurn 3 times.", turn_example},
      {"Drive and Turn\n\nDrive forward, turn, come back", drive_and_turn},
      {"Drive and Turn\n\nSlow down during drive", wait_until_change_speed},
      {"Swing Turn\n\nSwing in an 'S' curve", swing_example},
      {"Motion Chaining\n\nDrive forward, turn, and come back, but blend everything together :D", motion_chaining},
      {"Combine all 3 movements", combining_movements},
      {"Interference\n\nAfter driving forward, robot performs differently if interfered or not", interfered_example},
      {"Simple Odom\n\nThis is the same as the drive example, but it uses odom instead!", odom_drive_example},
      {"Pure Pursuit\n\nGo to (0, 30) and pass through (6, 10) on the way.  Come back to (0, 0)", odom_pure_pursuit_example},
      {"Pure Pursuit Wait Until\n\nGo to (24, 24) but start running an intake once the robot passes (12, 24)", odom_pure_pursuit_wait_until_example},
      {"Boomerang\n\nGo to (0, 24, 45) then come back to (0, 0, 0)", odom_boomerang_example},
      {"Boomerang Pure Pursuit\n\nGo to (0, 24, 45) on the way to (24, 24) then come back to (0, 0, 0)", odom_boomerang_injected_pure_pursuit_example},
      {"Measure Offsets\n\nThis will turn the robot a bunch of times and calculate your offsets for your tracking wheels.", measure_offsets},
  });

  // Initialize chassis and auton selector
  chassis.initialize();
  ez::as::initialize();

  liftMotor.tare_position();
  liftRotation.reset_position();
  pivotRotation.reset_position();
  lift.state = Lift::ZERO;
  // Started here rather than at static init - chassis, lift and intake live in
  // config.cpp now, and a file scope task could run before their constructors do.
  ezScreenTask = new pros::Task(ez_screen_task);
  liftTask = new pros::Task([] { lift.liftControl(); });
  intakeTask = new pros::Task([] { intake.intakeControl(); });

  controller.rumble(chassis.drive_imu_calibrated() ? "." : "---");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  // . . .
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
  // . . .
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  chassis.pid_targets_reset();                        // Resets PID targets to 0
  chassis.drive_imu_reset();                          // Reset gyro position to 0
  chassis.drive_sensor_reset();                       // Reset drive sensors to 0
  chassis.odom_xyt_set(0_in, 0_in, 0_deg);            // Set the current position, you can start at a specific position with this
  chassis.drive_brake_set(pros::E_MOTOR_BRAKE_HOLD);  // Set motors to hold.  This helps autonomous consistency

  // ez::as::auton_selector.selected_auton_call();  // Calls selected auton from autonomous selector
  // driver_side_three_pin();
  // drive_example();
  skillz();
  // turn_example();
}

/**
 * Simplifies printing tracker values to the brain screen
 */
void screen_print_tracker(ez::tracking_wheel* tracker, std::string name, int line) {
  std::string tracker_value = "", tracker_width = "";
  // Check if the tracker exists
  if (tracker != nullptr) {
    tracker_value = name + " tracker: " + util::to_string_with_precision(tracker->get());             // Make text for the tracker value
    tracker_width = "  width: " + util::to_string_with_precision(tracker->distance_to_center_get());  // Make text for the distance to center
  }
  ez::screen_print(tracker_value + tracker_width, line);  // Print final tracker text
}

/**
 * Ez screen task
 * Adding new pages here will let you view them during user control or autonomous
 * and will help you debug problems you're having
 */
void ez_screen_task() {
  while (true) {
    // Only run this when not connected to a competition switch
    if (!pros::competition::is_connected()) {
      // Blank page for odom debugging
      if (chassis.odom_enabled() && !chassis.pid_tuner_enabled()) {
        // If we're on the first blank page...
        if (ez::as::page_blank_is_on(0)) {
          // Display X, Y, and Theta
          ez::screen_print("x: " + util::to_string_with_precision(chassis.odom_x_get()) +
                               "\ny: " + util::to_string_with_precision(chassis.odom_y_get()) +
                               "\na: " + util::to_string_with_precision(chassis.odom_theta_get()),
                           1);  // Don't override the top Page line

          // Display all trackers that are being used
          screen_print_tracker(chassis.odom_tracker_left, "l", 4);
          screen_print_tracker(chassis.odom_tracker_right, "r", 5);
          screen_print_tracker(chassis.odom_tracker_back, "b", 6);
          screen_print_tracker(chassis.odom_tracker_front, "f", 7);
        }
      }
    }

    // Remove all blank pages when connected to a comp switch
    else {
      if (ez::as::page_blank_amount() > 0)
        ez::as::page_blank_remove_all();
    }

    pros::delay(ez::util::DELAY_TIME);
  }
}

/**
 * Gives you some extras to run in your opcontrol:
 * - run your autonomous routine in opcontrol by pressing DOWN and B
 *   - to prevent this from accidentally happening at a competition, this
 *     is only enabled when you're not connected to competition control.
 * - gives you a GUI to change your PID values live by pressing X
 */
void ez_template_extras() {
  // Only run this when not connected to a competition switch
  if (!pros::competition::is_connected()) {
    // Disabled so pressing X for raw lift up doesn't turn on the PID tuner
    // if (controller.get_digital_new_press(DIGITAL_X))
    //   chassis.pid_tuner_toggle();

    // Trigger the selected autonomous routine
    if (controller.get_digital(DIGITAL_B) && controller.get_digital(DIGITAL_DOWN)) {
      pros::motor_brake_mode_e_t preference = chassis.drive_brake_get();
      autonomous();
      chassis.drive_brake_set(preference);
    }

    // Allow PID Tuner to iterate
    chassis.pid_tuner_iterate();
  }

  // Disable PID Tuner when connected to a comp switch
  else {
    if (chassis.pid_tuner_enabled())
      chassis.pid_tuner_disable();
  }
}

/**
 * Driver layout:
 *   R1 / R2       intake in / out
 *   L1 / L2       lift up / down a level - LEVEL_1 is on the front, every other level goes to the back
 *   L1 + L2       toggle Front / Back - keeps the same level (zero or LEVEL_1 go to back LEVEL_2).
 *                 The only way to the front LEVEL_2 / LEVEL_3.
 *                 Press both within chordWindow (100ms) of each other.
 *   UP            lift to LOADING_HEIGHT, arm to LOADING_ANGLE
 *   A             lift to MATCHLOADING_HEIGHT, arm to FRONT_ANGLE once the lift is below MATCHLOADING_PIVOT_HEIGHT
 *   X / B         raw lift up / down (hold)
 *   RIGHT / DOWN  raw pivot arm control (hold to move, release to hold angle)
 *   LEFT          score - drop, open claw, then up to scoreRise above where it was (at zero: only opens the claw)
 *   Y             open / close claw
 */
void opcontrol() {
  chassis.drive_brake_set(pros::E_MOTOR_BRAKE_COAST);
  // Don't reset lift.isBackMode here - opcontrol restarts after every disable / enable, and the
  // lift would drive itself from back LEVEL_2-3 to the front.  The level buttons set the side.

  // L1 / L2 tap tracking - see "Lift Levels" below
  bool l1Last = false;
  bool l2Last = false;
  bool toggled = false;         // L1 + L2 already toggled - let go of both before the next toggle or tap
  int pendingTap = 0;           // 1 = L1 waiting to go up a level, -1 = L2 waiting to go down, 0 = none
  int tapTime = 0;              // ms the pending tap has waited
  const int chordWindow = 100;  // ms a tap waits for the other button before it moves a level

  while (true) {
    ez_template_extras();
    chassis.opcontrol_arcade_standard(ez::SPLIT);

    if (controller.get_digital_new_press(DIGITAL_LEFT))
      lift.score();

    // Intake
    if (controller.get_digital(DIGITAL_R2)) {
      // intake.set(Intake::INTAKE);
      toggleMotor.move(0);
    } else if (controller.get_digital(DIGITAL_R1)) {
      // intake.set(Intake::OUTTAKE);
      toggleMotor.move(60);
    } else {
      // 'p/intake.set(Intake::STOP);
      toggleMotor.move(0);
    }

    // Lift Levels & Front/Back Toggle (L1 + L2)
    // A tap waits until it's let go, or chordWindow ms, before moving a level.  If the other
    // button comes down in that time it's a toggle instead, so the toggle never moves a level first.
    bool l1 = controller.get_digital(DIGITAL_L1);
    bool l2 = controller.get_digital(DIGITAL_L2);
    if (l1 && l2) {
      if (!toggled) lift.toggleFrontBack();
      toggled = true;
      pendingTap = 0;
    } else if (!l1 && !l2) {
      toggled = false;
    }
    if (!toggled) {
      if (pendingTap != 0) {
        tapTime += ez::util::DELAY_TIME;
        bool held = pendingTap > 0 ? l1 : l2;
        if (!held || tapTime >= chordWindow) {
          if (pendingTap > 0) {
            lift.levelUp();
          } else {
            lift.levelDown();
          }
          pendingTap = 0;
        }
      }
      if (l1 && !l1Last) {
        pendingTap = 1;
        tapTime = 0;
      } else if (l2 && !l2Last) {
        pendingTap = -1;
        tapTime = 0;
      }
    }
    l1Last = l1;
    l2Last = l2;

    if (controller.get_digital_new_press(DIGITAL_UP))
      lift.set(Lift::LOADING);

    if (controller.get_digital_new_press(DIGITAL_A)){
      claw.set(true);
      lift.set(Lift::MATCHLOADING);
    }

    // Raw Lift (X up / B down)
    // Only set() on the change - set() drops the manual arm hold, so calling it every frame
    // would make the arm spring back after RIGHT / DOWN while X or B is held
    if (controller.get_digital(DIGITAL_X)) {
      if (lift.state != Lift::RAW_UP) lift.set(Lift::RAW_UP);
    } else if (controller.get_digital(DIGITAL_B) && !controller.get_digital(DIGITAL_DOWN)) {
      if (lift.state != Lift::RAW_DOWN) lift.set(Lift::RAW_DOWN);
    } else if (lift.state == Lift::RAW_UP || lift.state == Lift::RAW_DOWN) {
      lift.set(Lift::RAW_HOLD);
    }

    // Manual Pivot (Hold to move, release to lock angle)
    if (controller.get_digital(DIGITAL_RIGHT)) {
      lift.pivotRawPower = -90;
    } else if (controller.get_digital(DIGITAL_DOWN) && !controller.get_digital(DIGITAL_B)) {
      lift.pivotRawPower = 90;
    } else {
      lift.pivotRawPower = 0;
    }

    // Claw Toggle
    if (controller.get_digital_new_press(DIGITAL_Y)) {
      claw.set(!claw.get());
    }

    pros::delay(ez::util::DELAY_TIME);
  }
}