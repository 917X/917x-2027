#pragma once
#include "EZ-Template/piston.hpp"
#include "pros/motor_group.hpp"

/**
 * Lift + claw subsystem.
 *
 * Wraps the lift motor group and the two pistons (claw and claw pivot) so the
 * driver macros live in one place instead of as free functions in main.cpp.
 *
 * NOTE: every motion here is blocking - the macros call pros::delay() inline,
 * exactly as they did in the early season code.  Pressing a macro button stalls
 * the opcontrol loop (drive included) until the macro finishes.
 */
class Lift {
   public:
    Lift(pros::MotorGroup& motors, ez::Piston& claw, ez::Piston& pivot);

    // Timings / powers, carried over from the early season tuning
    static constexpr int MACRO_TIME_MS = 1300;      // full travel
    static constexpr int CLEARANCE_TIME_MS = 250;   // just enough to clear the claw
    static constexpr int PNEUMATIC_DELAY_MS = 250;  // let a piston finish before moving on
    static constexpr int HOLD_POWER = 20;
    static constexpr int RAISE_POWER = 127;
    static constexpr int LOWER_POWER = -63;  // was -63.5, truncated to int by MotorGroup::move()
    static constexpr int MANUAL_UP_POWER = 127;
    static constexpr int MANUAL_DOWN_POWER = -30;

    static constexpr bool PIVOT_STARTING_STATE = false;
    static constexpr bool PIVOT_INTAKE_STATE = true;

    /**
     * Sets the lift to hold its position.  Call once from initialize().
     */
    void initialize();

    // Low level
    void moveFor(int power, int duration_ms);  // blocking move, then falls back to hold power
    void manual(int power);                    // direct passthrough for the L1/L2 driver buttons

    // Lift positions
    void raiseFull();
    void lowerFull();
    void raiseClearance();
    void lowerClearance();

    // Pneumatics
    void setPivot(bool state);
    void togglePivot();
    void openClaw();
    void closeClaw();
    void toggleClaw();

    // Driver macros
    void intake();     // clear, pivot out, open claw - ready to grab
    void matchload();  // clear, pivot home, open claw - ready to load
    void raise();      // grab, pivot home, lift up
    void lower();      // pivot home, lift down

    pros::MotorGroup& motors;
    ez::Piston& claw;
    ez::Piston& pivot;

    /**
     * Power moveFor() leaves the lift at once its blocking move is done.
     *
     * The opcontrol manual block writes to the lift every cycle, so in driver
     * control this gets overwritten with 0 on the next loop and the HOLD brake
     * mode is what actually keeps the lift up.  It does hold during autons.
     */
    int holdPower = 0;
};
