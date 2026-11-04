---
agent: agent
description: "Write an Implementation doc for one subsystem or chassis feature from its current source code, in the 917X notebook format."
argument-hint: "Subsystem or feature name, e.g. Lift"
---

# Write an Implementation Doc

**Subsystem / feature:** ${input:subsystem:Subsystem or feature name, e.g. Lift}
**Category folder:** ${input:category:Subsystems or Chassis (default Subsystems)}
**Document owner / designer / technical writer:** ${input:owner:Full name(s); one name is used for all three}
**Target release:** ${input:targetRelease:YYYY-MM-DD, or blank for TBD}

If any value above is blank or still shows a literal `${input:...}`, apply these defaults: **subsystem** → ask the user (it's the only thing you must ask), **category** → `Subsystems`, **owner** → `TBD`, **target release** → `TBD`.

An Implementation doc explains **how the code works** well enough that a new programmer could maintain, tune, and debug it without asking anyone. It documents **existing code only**. If the code doesn't exist yet, stop and tell the user to write the PRD first (`doc-prd.prompt.md`).

---

## Step 0: Load the rules and examples

1. Read `.github/instructions/documentation.instructions.md` **in full**. §6 (Accuracy) overrides everything else.
2. Read these examples for format (commands are in §1 of the rules file): **Intake Implementation**, **PID Controller Implementation**, and **Slew Rate Limiter Implementation**. For anything math-heavy, also read **IMU Fusion Implementation**.
3. If `DOC_ROOT/<category>/<Name> PRD.md` exists, read it. Its Success Metrics drive the Alignment section, and its names must match yours.

## Step 1: Gather facts

Run the §10 Fact-Gathering Procedure and build the complete fact sheet. Don't start writing until you have read every source file you will cite.

## Step 2: Choose the tier

Use **Full** if **any** of these are true. Otherwise use **Compact**.
- More than one actuator (motor, piston) is controlled.
- There is closed-loop control (P, PID, or a filter).
- There is an automatic multi-phase sequence (homing, a scoring macro, an auto-stop).
- There are 5 or more tunable constants.
- There is safety logic (soft limits, current or stall detection, timeouts).

| **Tier** | **Model doc**                              | **Typical length** |
|:---------|:-------------------------------------------|:-------------------|
| Compact  | Intake Impl., Slew Rate Limiter Impl.      | 150–300 lines      |
| Full     | PID Controller Impl., Odometry Impl.       | 400–900 lines      |

## Step 3: Write the doc

Path: `DOC_ROOT/<category>/<Name> Implementation.md`.

### Skeleton (keep this order)

`[C]` = required in Compact and Full. `[F]` = required in Full only. `[if]` = include only when its condition holds.

```markdown
# <Name> Implementation

<Implementation metadata table from §2>

## ▲ In This Document:                          [C]
## ▲ Executive Summary                          [C]
### Purpose & Need
### Why a <Pattern>? / How It Works / Mathematical Foundation / Credit   [if]
## ▲ Architecture                               [C]
### External Dependencies
### Internal Dependencies
### Interfaces
## ▲ Design Spec                                [C]
## ▲ Implementation Overview                    [C]
### File Structure
### Key API
### Driver Control Mapping                      [if the controller drives it]
### Usage Examples
### State Behavior Summary                      [if it has states]
## ▲ Data Structures                            [F]
## ▲ Parameter Reference                        [F, or if it has tunable constants]
## ▲ Implementation Explainer                   [F]
## ▲ Control Loop Flowchart                     [F]
## ▲ Tuning Methodology                         [F, or if it has tunable constants]
## ▲ Troubleshooting                            [F]
## ▲ Key Design Decisions                       [F]
## ▲ Integration with Existing Documentation    [if related docs exist]
## ▲ Alignment with PRD Goals                   [if a PRD exists]
## ▲ Key Takeaways                              [F]
## ▲ References and Further Reading             [if external sources were used]
## ▲ Tests & Performance Analysis               [C] fixed placeholder
## ▲ Issues                                     [C] fixed placeholder
## ▲ Version History                            [C] fixed placeholder
```

### Section requirements

**In This Document:** 4–9 bullets of noun phrases listing what the doc covers, in order. The first bullet names the class in backticks, for example "The `Lift` class structure and finite state machine design".

**Executive Summary**
- `### Purpose & Need` (2–4 bullets): the physical task and why it is hard, how the class solves it, and the result.
- `### Why a Finite State Machine?` (for any subsystem with states): explain what goes wrong with the naive approach (calling `motor.move()` at every call site leads to conflicting commands and no single source of truth). Then give the benefits as bold-led bullets: **One control loop, one source of truth.**, **Concurrency safe.**, **Extensible.** Tie each one to this subsystem.
- `### Mathematical Foundation`: use it when one equation is the heart of the system. Include the **Where:** list.
- `### Credit`: required whenever an algorithm or code was adapted from an external source. Link it.

**Architecture**
- `### External Dependencies`, with columns `Component | Type | Purpose`. Types are `Kernel Library` (`pros::`), `EZ-Template` (`ez::`), `Measurement Unit Helper` (`okapi::` units), and `C++ Standard Library` (`std::`). List only what the code actually uses.
- `### Internal Dependencies`, with columns `Component | Type | Purpose`. Cover project classes, objects, and functions it relies on, adding "(See <X> Entry)" where a doc exists. If there are none, write one sentence saying the class is self-contained.
- `### Interfaces`, with columns `Type | Name` and **exactly two rows**: `Component Input` (the commands and sensor readings it consumes) and `Component Output` (what it commands: motor power, piston state, values exposed).

**Design Spec**

This is the conceptual design, explained before the line-by-line walkthrough. For a **subsystem with states**, include all of these that apply, in this order:

1. `### Finite State Machine`: the copied enum excerpt, then a table `State | <Actuator 1> | <Actuator 2> | ... | Purpose` with one row per state and one column per actuator, showing what each actuator targets or outputs in that state. Group the states and say how they are grouped (for example: position states, manual/raw states, automatic sequences).
2. `### State Transitions`: the external transitions (`set()` and each helper method, with its guard conditions and entry actions such as reset counters or a latched start position), then the **automatic** transitions made inside the loop. Summarize them in a table `From | Trigger | To | Notes`.
3. `### Control Law` (one subsection per closed-loop actuator): the equation with its clamp, the **Where:** list mapping symbols to code names with units, and the **Intuition:** paragraph. Give the gain's units, such as "power per degree of error".
4. `### Safety Limits`: soft limits, timeouts, and anything that refuses to drive the hardware. Explain what each one protects against.
5. One `###` per **automatic sequence** (for example `### Homing Sequence`, `### Score Sequence`): the purpose, the physical reasoning (quote the code comments when they explain it), a phase table `Phase | Action | Exit Condition | Next`, and the constants involved. Add an inline `[Note: Insert diagram here — ...]` timing or flowchart instruction.
6. `### Background Task Model`: where the task is launched (cite the file and function), the loop period and frequency, why callers never block, and how state written by `set()` is picked up on the next tick.

For a **non-state feature** (an algorithm or controller), use `### Step N: <Title>` subsections that build the mechanism up one idea at a time, the way the Odometry and IMU Fusion docs do.

Every Design Spec code excerpt follows the excerpt rules in §4 of the rules file.

**Implementation Overview**
- `### File Structure`: a `txt` tree containing only the relevant real files, each with a `#` comment. It always includes the header, the source, `src/config.cpp` (construction and ports), and `src/main.cpp` (task launch and controls) when those apply.
- `### Key API`, with columns `Function / Member | Parameters | Description`: every public method, plus the public tunables a user would set.
- `### Driver Control Mapping`, with columns `Button | Condition | Call | Effect`: use the exact `pros::E_CONTROLLER_DIGITAL_*` names, and say whether a row fires on press, new press, hold, or release. Copy the mapping from `opcontrol()`, not from comments, and note any place where the comments disagree with the code.
- `### Usage Examples`:
  - **Driver control:** a real excerpt from `opcontrol()`.
  - **Autonomous:** a real excerpt from `src/autons.cpp`, but **only if it compiles against the current API**. Otherwise write a minimal example that does, and introduce it with *Example usage (not taken from autons.cpp).* If the subsystem has no blocking wait helper, show how a routine waits for it (a delay, or polling a sensor against the target), and add a TODO suggesting a wait helper if one is clearly needed.
- `### State Behavior Summary`: a compact recap table, one row per state, for quick reference.

**Data Structures** `[F]`: one `cpp` block per group, copied from the header with explanatory comments. Typical groups: hardware references; state variables; control constants; per-sequence working variables; position tables (heights and angles); safety limits.

**Parameter Reference**, with columns `|  **Parameter**  |  **Default**  |  **Unit**  |  **Effect**  |  **Status**  |`
- One row for **every** tunable constant.
- Effect says what raising the value does, and what lowering it does where that differs.
- Status is `Tuned` or `Placeholder`. Mark it Placeholder whenever the code says so (§6.4).
- Add a `Typical Range` column only when a range comes from the code, the comments, or the user.

**Implementation Explainer** `[F]`: `### Section I. <Title>`, `### Section II. ...`, walking the code in execution order: construction → task start and initialization → loop structure → per-state target selection → control law and clamping → safety limits → each automatic sequence → public API methods. In each section, write a 1–2 sentence lead-in, then the excerpt, then an explanation. Close any section that contains non-obvious choices with a **Key Design Decisions:** numbered list.

**Control Loop Flowchart** `[F]`: `**Flowchart Instructions:**` for **one iteration** of the control loop: reading the sensors → the state switch → computing power → limits → special-state overrides → writing the outputs → delay. Follow §7 of the rules file. If an automatic sequence is complex, add a separate `## ▲ <Sequence> Flowchart` or timing diagram.

**Tuning Methodology**
- Write `### Step N: <Title>` steps the team can follow **on the robot**, in a safe order. Usually that is: verify sensor direction and zero → set soft limits → tune each gain starting low → set the position constants → tune the sequence thresholds. For each step, say what to change, what to watch, how to adjust (direction and rough proportion, such as "raise `kP` in small steps"), and when it is done.
- If the code prints tuning telemetry (for example `printf` during homing), say how to read it.
- End with `### Common Tuning Issues`, with columns `Symptom | Likely Cause | Solution`.

**Troubleshooting** `[F]`: 4–7 entries in the form `### <Symptom stated as a problem>`, each with **Symptoms:**, **Causes:** (bullets), and **Solutions:** (a code or config change in a `cpp` block, or a physical check). Cover **faults**, such as a reversed sensor or motor, the task not started, a wrong port, a state never reached, limits blocking motion, or a sequence that never exits. Tuning *performance* belongs in Tuning Methodology. Derive every entry from what the code would actually do in that failure.

**Key Design Decisions** `[F]`: `### N. <Decision>`, each followed by "We chose <X> because:" and 2–4 bold-led bullets. Include the choices the PRD's Decision Matrix covered, plus the significant code-level ones.

**Integration with Existing Documentation**: one `**<Entry Name>:**` paragraph or bullet list for each related entry that exists or is being written, saying how this system uses it or depends on it.

**Alignment with PRD Goals**, with columns `|  **Goal**  |  **Implementation**  |  **Status**  |`
- The Goal column copies the PRD's Success Metrics Goals **word for word**, one row each.
- Implementation names where and how the code satisfies the goal.
- Status is honest: `✅ Met`, `⚠️ Partial` (for example, the logic is there but the constants are placeholders), or `❌ Not Met`.

**Key Takeaways** `[F]`: 4–8 numbered items, each a **bold lead phrase** followed by an explanation. These are the ideas a new programmer must keep.

**Tail placeholders:** `Tests & Performance Analysis`, `Issues`, and `Version History`, each containing only the fixed placeholder line from §8 of the rules file.

## Step 4: Check

Run the §11 Final Checklist from the rules file, plus:
- [ ] Every enum value appears in the state table, and every transition in the code appears in the transitions table.
- [ ] Every public method appears in Key API, and every tunable constant appears in Parameter Reference.
- [ ] The Driver Control Mapping matches the `opcontrol()` code.
- [ ] Every code excerpt matches the source file it names (diff it mentally line by line).
- [ ] The autonomous example compiles against the current header.

## Step 5: Report (in chat, not in the doc)

- The file path and the tier used.
- Every `TODO` left in the doc.
- Every code discrepancy found (§6.7), with its `file:line`.
- Constants documented as placeholders.
- Design reasons you inferred rather than found, so the owner can confirm them.
