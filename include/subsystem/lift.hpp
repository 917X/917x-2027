#pragma once
#include <cstdint>
#include "EZ-Template/piston.hpp"
#include "pros/motor_group.hpp"
#include "pros/rtos.hpp"

/**
 * Lift + claw subsystem.
 *
 * A macro is a short script of steps - move the lift, fire a piston, wait a bit -
 * and liftControl() walks that script from a background task.  Starting a macro
 * returns immediately, so the drive keeps running while the lift moves.
 *
 * Start the task once in initialize():
 *
 *     liftTask = new pros::Task([] { lift.liftControl(); });
 *
 * In an auton, follow a macro with waitUntilDone() when the next movement
 * depends on the lift being finished:
 *
 *     lift.raise();
 *     lift.waitUntilDone();
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

    static constexpr int UPDATE_DELAY_MS = 10;      // how often liftControl() advances a script
    static constexpr int DEFAULT_TIMEOUT_MS = 5000; // waitUntilDone() gives up after this

    static constexpr bool PIVOT_STARTING_STATE = false;
    static constexpr bool PIVOT_INTAKE_STATE = true;

    enum Macro {
        NONE,
        INTAKE,     // clear, pivot out, open claw - ready to grab
        MATCHLOAD,  // clear, pivot home, open claw - ready to load
        RAISE,      // grab, pivot home, lift up
        LOWER,      // pivot home, lift down
        RAISE_FULL,
        LOWER_FULL,
        RAISE_CLEARANCE,
        LOWER_CLEARANCE
    };

    /**
     * One step of a macro: do the thing, then wait duration_ms before the next step.
     */
    struct Step {
        enum Kind { MOVE, PIVOT, CLAW };
        Kind kind;
        int value;        // MOVE: motor power.  PIVOT / CLAW: piston state
        int duration_ms;
    };

    /**
     * Sets the lift to hold its position.  Call once from initialize().
     */
    void initialize();

    /**
     * Task body.  Loops forever calling update().
     */
    void liftControl();

    /**
     * Advances the running macro.  liftControl() calls this for you every
     * UPDATE_DELAY_MS; call it yourself only if you'd rather drive the lift
     * from a loop you already have instead of running the task.
     */
    void update();

    // Macros.  All return immediately - liftControl() does the work.
    // Starting one while another is running replaces it.
    void run(Macro macro);
    void intake();
    void matchload();
    void raise();
    void lower();
    void raiseFull();
    void lowerFull();
    void raiseClearance();
    void lowerClearance();

    /**
     * Stops the running macro and the lift with it.  Safe to call when idle.
     */
    void cancel();

    bool busy();

    /**
     * Blocks until the running macro finishes.  Cancels and returns if it takes
     * longer than timeout_ms, so a stalled macro can't hang an auton.
     */
    void waitUntilDone(int timeout_ms = DEFAULT_TIMEOUT_MS);

    /**
     * Driver lift control.  A nonzero power cancels any running macro - the
     * driver always wins.  Zero is ignored while a macro is running so the
     * opcontrol loop doesn't stomp on it every cycle.
     */
    void manual(int power);

    // Pneumatics, applied immediately.  The settle delays live in the macro
    // scripts, where the next step actually depends on them.
    void setPivot(bool state);
    void togglePivot();
    void setClaw(bool state);
    void toggleClaw();
    void openClaw();
    void closeClaw();

    pros::MotorGroup& motors;
    ez::Piston& claw;
    ez::Piston& pivot;

   private:
    static const Step* scriptFor(Macro macro, int& length);

    pros::Mutex mutex;
    Macro active = NONE;
    const Step* script = nullptr;
    int scriptLength = 0;
    int stepIndex = 0;
    std::uint32_t stepStart = 0;
    bool stepStarted = false;

    /**
     * Power a MOVE step leaves the lift at once it finishes.
     *
     * In driver control the opcontrol manual block writes 0 on the next cycle,
     * so the HOLD brake mode is what actually keeps the lift up.  It does hold
     * during autons, where nothing else is writing to the lift.
     */
    int holdPower = 0;
};
