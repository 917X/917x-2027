#pragma once
#include "EZ-Template/util.hpp"
#include "subsystem/lift.hpp"

void default_constants();

void claw_autoclose(int timeout_ms, bool lift_pivot_after = false);
void claw_autoclose(int timeout_ms, Lift::LiftState lift_state_after, bool close_on_timeout = true);
void claw_autoclose(ez::pose target, int timeout_ms, double tolerance_in = 1.0);

void drive_example();
void turn_example();
void drive_and_turn();
void wait_until_change_speed();
void swing_example();
void motion_chaining();
void combining_movements();
void interfered_example();
void odom_drive_example();
void odom_pure_pursuit_example();
void odom_pure_pursuit_wait_until_example();
void odom_boomerang_example();
void odom_boomerang_injected_pure_pursuit_example();
void measure_offsets();
void test_autoclose();
void skillz();
void sawpKinda();
// void driver_side_three_pin();