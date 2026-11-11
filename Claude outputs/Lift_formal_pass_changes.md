# Lift entry: formal-style pass (change log)

Deletions are ~~struck through~~ and additions are **bold**. Each changed sentence is shown in full. Code, diagram labels and values were not changed.

## Page 581
- It ~~is built from~~ **consists of** three mechanisms: a string-winch lift driven by one motor, a pivoting arm driven by a second motor, and a pneumatic claw that grips and releases.
- The lift is a stateful system, so the implementation should follow the conventions of a finite state machine (FSM), ~~like~~ **as with** the Intake.
- ~~Swing~~ **Rotate** the arm to the angle that ~~goes with~~ **corresponds to** that height
- Move freely while the driver holds a button, then hold ~~where it stops~~ **its position on release**
- ~~Find~~ **Locate** the bottom ~~on its own~~ **of travel** (homing)
- ~~Similar to~~ **As with** the Intake’s states, all of these **behaviors** are finite states. ~~A~~ **Each** state sets the lift height and the arm angle together, so the lift is ~~in~~ **always in exactly** one pose ~~at a given moment~~ and the driver changes it with ~~one button.~~ **a single button press.**

## Page 582
- It ~~does~~ **accomplishes** this with three ~~mechanisms working together.~~ **coordinated mechanisms.** The code controls each ~~one~~ **mechanism** through an ~~actuator, and~~ **actuator and,** where possible, reads its position from a sensor.
- The lift must maintain its height against gravity while reaching ~~exact heights every time.~~ **the target height consistently.**
- ~~To work the way our driver prefers,~~ **Based on the driver’s preferences,** the lift ~~must follow~~ **follows** a four-step scoring cycle:
- Raise: The claw closes on the object, and the lift rises to one of the six scoring heights while the pivot arm ~~turns~~ **rotates** to the angle ~~where~~ **at which** the pin stands vertical, **known as** the scoring angle.
- Score: The lift drops slightly onto the goal, the claw opens, and the lift rises clear. This is **referred to as** the drop, open, rise sequence.
- NOTE: Each step moves the arm and lift together. A lift height combined with its matching arm angle is called a pose. Since the lift and arm must always ~~agree on one~~ **remain in the same** pose, both ~~run in~~ **are controlled by** the same finite state machine.

## Page 583
- The driver can move the lift ~~manually and have it hold where~~ **manually, and the lift holds its position when** released
- The loading, matchloading and scoring poses each set their own angle, measured by the arm rotation sensor. States without an ~~angle keep the last one~~ **assigned angle hold the previous angle**
- ~~One~~ **A single** press scores a game object
- A score command drops the lift, opens the claw, ~~rises clear~~ **raises the lift clear of the goal** and holds. ~~Every move has a time limit~~ **Each move is limited by a timeout**
- A homing routine detects the sharp rise in motor current once the lift passes its lowest point, and zeros the lift ~~there~~ **sensor at that point**
- The code ~~cuts~~ **zeroes** any power that would drive the lift or arm past a set limit
- liftControl() runs in its own PROS task, as ~~on~~ **with** the intake, and set() returns immediately

## Page 584
- Each requirement is a feature the code must ~~have~~ **implement** to meet the Success Metrics. HIGH ~~means~~ **indicates that** the lift cannot be used in a match without it.
- Finite state machine with fourteen ~~states, one per~~ **states covering every** action in the Problem Statement
- Same structure as the intake: liftControl() ~~loops~~ **runs** in its own PROS task and never blocks driver control or autonomous
- A score command, score(), runs the drop, open, rise sequence and then holds **its position**
- A soft limit is a position past which the code will not drive the motor. Power back ~~toward the~~ **into** range is still allowed

## Page 585
- Sequences (right) run their steps automatically; each sequence is outlined on the following pages. The dashed arrows ~~are the transitions the program makes automatically~~ **represent the automatic transitions that occur** once a sequence finishes.

## Page 586
- Scoring ~~height~~ **heights** 1 to 6 ~~(after testing through hand turns, we found each~~ **(measured by turning the lift by hand; each level** is 2000 degrees apart)
- The lift’s soft limits ~~sit~~ **are set** 10 degrees below the bottom of travel and about 500 degrees above the sixth scoring height. The arm has ~~its own~~ **a separate** pair of soft limits.
- As outlined in the Objective, a pose pairs one lift height with one arm angle, ~~much like~~ **similar to** an (x, y) ~~coordinate.~~ **coordinate pair.** Each pose state sets exactly one ~~of each.~~ **height and one angle.**
- The ~~other~~ **remaining** states are behaviors. ~~They set~~ **Each one sets** the lift power directly or ~~run~~ **runs** a sequence, ~~and~~ **while** the arm ~~keeps~~ **holds** its last angle.

## Page 587
- SCORE automates step 3 of the scoring cycle (the score motion) when score() is called. The arm ~~keeps~~ **holds** its current angle ~~throughout.~~ **for the entire sequence.**
- Release. The claw opens, and the lift waits a set release time while the claw ~~lets go.~~ **releases the object.** If the claw is already open, this step is skipped.
- Hold. The lift switches to RAW_HOLD and ~~stays at that height. Returning~~ **holds that height, since returning** to a level preset would lower it back onto the goal.
- Steps 2 to 4 are ~~called~~ **referred to as** the Dropping, Releasing and Raising phases.
- A move ~~counts as~~ **is considered** complete when the lift is within a set tolerance of its target or when a move timeout has passed. The timeout prevents any step from stalling the sequence.

## Page 588
- HOMING ~~finds the lowest point of the lift by watching for the current rise~~ **locates the bottom of travel by detecting the rise in motor current** as the lift lowers. The rise was measured on the robot by driving the lift down at a power of 45 and printing the current and speed every 50 ms. ~~Below are the results of our testing:~~ **The results are shown below:**
- ~~Lifting the lift~~ **Under load**
- The current roughly triples while the speed drops **by** only about 15%. ~~Since~~ **As** the larger change is less prone to noise, ~~we use~~ the current **is used** to identify ZERO. Both thresholds ~~sit~~ **lie** between the two ranges.
- then ~~jumps while~~ **spikes as** the motor ~~speeds up~~ **accelerates**

## Page 589
- ~~How~~ **The organization of** the Lift class and its fourteen-state FSM ~~are organized~~
- How ~~one~~ **the** control loop, liftControl(), converts the state into lift and arm power
- The ~~code behind~~ **implementation of** the SCORE and HOMING states
- The lift performs four ~~kinds of work~~ **types of tasks** (holding a pose, moving freely, scoring and homing) using three mechanisms: the lift motor, the arm motor and the claw piston.
- The lift follows the same FSM pattern as the ~~intake: other~~ **intake. External** code only requests which state the lift should be in, and the control loop alone ~~decides~~ **determines** what the motors do in that state.
- Calling motor.move() ~~straight~~ **directly** from each button would cause the same two problems outlined in the Intake entry: conflicting **motor** commands, now ~~just with~~ **across** two motors, and no single record of ~~what the lift is doing.~~ **the lift’s current behavior.** The FSM ~~avoids~~ **addresses** both:
- Only liftControl() drives the lift and arm motors. Buttons and autonomous routines ~~just~~ **only** request a state.
- ~~With set() as~~ **Since set() is** a single assignment, a button press or autonomous call ~~doesn’t~~ **does not** need to wait for the lift to arrive. The control loop ~~simply picks up~~ **applies** the new state on its next tick.
- Each pose state holds both targets, which keeps the height and arm angle ~~matched.~~ **consistent.**
- Scoring and homing are ~~states with phases.~~ **multi-phase states.** They run ~~inside~~ **within** the same loop and do not ~~pause~~ **block** the rest of the program.
- The sensor-based FSM is the second design used this season. The earlier ~~design, and why it was replaced, is described next.~~ **design and the reasons for its replacement are described in the Development History.**

## Page 590
- Added the score button ~~for a quicker~~ **to shorten the** scoring cycle.
- ~~None, only~~ **None;** raw movement ~~from pressing &~~ **only, by pressing and** holding
- Releasing **the** button stops movement
- ~~Easy~~ **Low**
- With raw movement only, the driver had to press and hold a button to move the lift, ~~making it slow and difficult to reach~~ **which made reaching** scoring heights and ~~return~~ **returning to the loading position slow and difficult**
- The sensor-based FSM reaches the desired height with ~~one~~ **a single** button press, ~~letting~~ **allowing** the driver **to** focus on gameplay ~~instead of~~ **rather than** stopping the lift at the ~~right height by hand~~ **correct height manually**

## Page 591
- ~~Gives liftControl()~~ **Runs liftControl() in** its own background task
- Placeholder ports. The arm ports ~~stay~~ **remain** marked as placeholders until they are confirmed on the robot.
- Lift constructor. The devices are passed into Lift, so the class ~~doesn’t~~ **does not** pre-define ports. Changing a port only requires editing config.cpp.

## Page 592
- New levels ~~go~~ **must be added** directly after LEVEL_6 ~~to keep~~ **for** level stepping ~~working~~ **to work**
- ~~We list the~~ **The** pose states **are listed** first, in ~~height order. This lets~~ **order of height. This allows** the code **to** check for a pose state with state <= LEVEL_6 and **to** step one level with level + 1.
- Two class fields store the FSM’s memory: state ~~is~~ **records** what the lift is currently doing, and level is the last pose state selected. After raw control, state is RAW_HOLD, so levelUp() relies on level to find the next level.

## Page 593
- Each ~~pass through~~ **iteration of** the loop is ~~one~~ **referred to as a** tick. Every tick begins with a 10 ms delay, so the loop runs at about 100 Hz.
- The ~~order matters~~ **execution order is significant** in two places:
- Homing (step 7) runs after the soft limits. It is the only state ~~allowed~~ **permitted** to drive below the bottom limit, which ~~it needs to find~~ **is necessary to locate** the true bottom. Once the bottom is found, the tick skips step 8 (dashed arrow).

## Page 594
- Sensor reads. Each sensor is read once per tick into position (lift height) and pivotPosition (arm angle). The sensors report centidegrees, so each reading is divided by 100 to ~~get~~ **convert it to** degrees.

## Page 595
- Each lift height from the Pose Table outlined previously is stored as a named constant. The arm angles are ~~covered~~ **documented** in Section VI.
- target ~~starts at~~ **is initialized to** the current position. Raw and homing states leave it unchanged, so their error is zero and P control adds no power (see Section V).
- pivotTarget is a class field rather than a loop variable, so it ~~keeps~~ **retains** its value between ticks. States that do not set it ~~keep~~ **retain** the last angle.

## Page 597
- Compute the error, the ~~distance left to travel:~~ **remaining distance to the target:** target - position.
- Multiply by kP. The power is ~~high~~ **large** far from the target and ~~drops~~ **decreases** near it, so the lift ~~slows~~ **decelerates** as it ~~arrives.~~ **approaches.**
- ~~Cap~~ **Clamp** the power between -speed and speed (PROS motor power ~~ranges from~~ **spans** -127 to 127).
- ~~Worked example:~~ **Example:** LOADING (1650°) to LEVEL_1 (2950°) gives an error of 1300°. With kP = 1.0 the power is capped at 127, so the lift runs at full power until it is within 127° of the target.
- Calculated on 11/3 as LEVEL_6_HEIGHT + 2500, ~~about~~ **approximately** where a seventh level would be. This leaves one level of travel for raw control and the score rise
- Chosen on 11/3, slightly below ZERO_HEIGHT, so raw down **control** stops before the string winds ~~the other way~~ **in the opposite direction**

## Page 598
- pivotTarget ~~starts at~~ **is initialized to** 0, and the arm sensor is zeroed at the arm’s starting position, so the arm holds its starting position until a state assigns it an angle.
- ~~We drive the arm~~ **The arm is driven** before the score and homing steps (steps 6 and 7), so it ~~keeps~~ **holds** its angle during both.

## Page 599
- Three ~~more~~ **additional** fields track progress: scoreStart (start height), scorePhase (current phase) and scoreTime (time in the phase). scoreTime ~~grows~~ **increases** by 10 ms each tick and resets to 0 when a phase begins.
- Dropping. Ends when the lift is within the tolerance of the dropped height or the timeout passes. claw.get() returns the last claw command: if the claw is closed, ~~we open it and move~~ **it is opened and the phase advances** to RELEASING; if it is already open, ~~we skip straight~~ **the phase advances directly** to RAISING.
- Releasing. Waits for the release time, then ~~moves~~ **advances** to RAISING.

## Page 600
- Reset scoreTime on every phase change, so each phase ~~gets~~ **has** its own timeout

## Page 601
- Three ~~more~~ **additional** fields track progress: homeTime (time since homing started), homeLoose (whether the string has gone slack) and loadedTime (the loaded timer).
- Power override. In HOMING, power is overridden to -homeSpeed. This ~~happens~~ **override is applied** after the soft limits, which ~~is why homing can~~ **allows homing to** drive below the bottom limit.
- Slack detection. The motor current (mA) is read every tick but ignored until homeTime ~~passes~~ **exceeds** the ignore time. The first time it drops below the slack threshold, homeLoose is set.
- Rise detection. While homeLoose is set and the current is above the loaded threshold, loadedTime ~~grows~~ **increases** by 10 ms per tick. Any tick below the threshold resets it to 0, so the rise must be sustained.
- Zeroing. Once loadedTime reaches the loaded time, ~~we stop the motor, zero~~ **the motor is stopped,** both the motor encoder and the lift rotation ~~sensor, and switch~~ **sensor are zeroed, and the state switches** to ZERO. continue skips the rest of that tick; every other tick ends by sending power to the lift motor (step 8).

## Page 603
- As with the intake, state transitions are handled by set(). Unlike the intake’s version, it ~~takes no speed.~~ **does not take a speed parameter.** If the new state is a pose state (<= LEVEL_6), it is also stored in level.
- levelUp() and levelDown() step from level rather than state, which ~~is why~~ **allows** level stepping ~~still works~~ **to function** after raw control or scoring.
- Stepping up from ZERO, MATCHLOADING or LOADING ~~goes~~ **moves** to LEVEL_1. Stepping up at LEVEL_6 has no effect.
- home() prepares the HOMING state **in** the same ~~way,~~ **manner,** clearing the homing timers. It has no effect if homing is already ~~running.~~ **in progress.**
- Simple ~~setter: the~~ **setter. The** control loop ~~picks up~~ **applies** the new state on its next tick

## Page 604
- Releasing X or B switches to RAW_HOLD only if the lift is in a raw state (RAW_UP or RAW_DOWN). Without ~~that~~ **this** check, every tick ~~with~~ **in which** X and B **are** released would cancel the current state.

## Page 605
- The lift task is created once, in initialize(), following the same pattern as the intake. ~~From then on,~~ **Thereafter,** liftControl() runs alongside all other code.
- The task is created after the devices in config.cpp are constructed. Its control loop ~~never waits on driver control,~~ and driver control ~~never waits on it.~~ **run independently and never wait on each other.**
- ~~None of the three rows waits on another: a~~ **The three tasks run independently. A** set() call from opcontrol() returns immediately, and the lift task ~~picks up~~ **applies** the new state on its next tick.

## Page 606
- Each test ~~maps~~ **corresponds** to one Success Metric; procedures follow on the next pages.
- The lift has not been tested on the robot yet. Results ~~are added once tested.~~ **will be recorded after testing.**
- T5 The score button scores a held ~~object:~~ **object using the** drop, open, rise **sequence**

## Page 607
- Y also toggles the claw, so press it again if the claw ~~needs to stay put~~ **must remain in its current state**
- ~~Keep the~~ **Use the same** pass margins ~~fixed between runs:~~ **for every run:** ±30° for the lift (matching the score tolerance) and ±5° for the arm
- Repeat for RIGHT, then press L1 six times for LEVEL_1 through LEVEL_6, **recording a** reading at each **level**
- Pass: every reading is within ±30° of its target and ~~holds~~ **remains** there
- Hold X for 1 s, release, then press L1; the lift should ~~go~~ **move** to LEVEL_4, ~~not~~ one level above the ~~raw position~~ **last level selected**
- Press LEFT to score, then press L2; the lift should ~~go~~ **move** to LEVEL_3

## Page 608
- Pass: the object is released onto the goal ~~every time,~~ **in every trial,** and the lift ends above its start height
- ~~Lower~~ **Reduce** rawSpeed for this test
- Hold B until the lift stops; the reading should be close to -10°, with the string still wound **in** the correct ~~way~~ **direction**
- At each end, press the opposite button to confirm **that** the lift ~~moves~~ **can move** back
- Pass: the lift stops at both limits without ~~hitting anything~~ **contacting a hard stop** and can always ~~move~~ **be driven** back
- Start homing and ~~watch~~ **monitor** the terminal for the current readings and homed
- Press Y twice; the reading should be ~~about~~ **approximately** 0
- Pass: homing stops at the true bottom without winding the string ~~the other way~~ **in the opposite direction**
- Pass: the ~~drive~~ **drivetrain** never pauses or stutters while the lift moves, ~~confirming~~ **which confirms that** the lift task runs in parallel

## Page 609
- Each row restates a Requirement as a question. Statuses ~~come from~~ **are based on** a code review on ~~11/3, not robot testing,~~ **11/3** and will be updated as ~~the~~ **robot** tests pass.
- P control on both rotation sensors. kP and pivotKP are placeholders and still ~~need~~ **require** tuning
- Routines call set() and score(), but autons.cpp still references clawPivot, clawIntake and Lift::ROLLERS, which no longer exist, so the project does not build. There is also no helper ~~to wait~~ **function that waits** for the lift to arrive
