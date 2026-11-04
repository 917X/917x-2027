---
agent: agent
description: "Write a PRD (Product Requirements Document) for one subsystem or chassis feature in the 917X notebook format."
argument-hint: "Subsystem or feature name, e.g. Lift"
---

# Write a PRD

**Subsystem / feature:** ${input:subsystem:Subsystem or feature name, e.g. Lift}
**Category folder:** ${input:category:Subsystems or Chassis (default Subsystems)}
**Document owner / designer / technical writer:** ${input:owner:Full name(s); one name is used for all three}
**Design brief (optional; required if the code doesn't exist yet):** ${input:brief:What it does, hardware, modes, driver controls, autonomous needs; or leave blank}

If any value above is blank or still shows a literal `${input:...}`, apply these defaults: **subsystem** → ask the user (it's the only thing you must ask), **category** → `Subsystems`, **owner** → `TBD`, **brief** → none.

A PRD answers **what** the system must do and **why** it is designed this way. **How** it is built belongs in the Implementation doc (`doc-implementation.prompt.md`). Keep the PRD between 50 and 150 lines.

---

## Step 0: Load the rules and examples

1. Read `.github/instructions/documentation.instructions.md` **in full**. Every rule there applies, and §6 (Accuracy) overrides everything else.
2. Read these example PRDs for format and tone (commands are in §1 of the rules file): **Intake PRD**, **PID Controller PRD**, **Driver Control PRD**. Copy their structure, never their facts.

## Step 1: Gather facts

- **Mode A (code exists):** run the §10 Fact-Gathering Procedure and build the fact sheet.
- **Mode B (no code yet, so this is a design PRD):** work from the brief. If the brief doesn't cover purpose, hardware (motors, sensors, pneumatics), modes or states, driver controls, autonomous needs, and safety constraints, ask the user about all the missing items **in a single message**, then continue. Mark any identifier that is not final with "(proposed)", for example `LiftState` (proposed).

## Step 2: Write the PRD

Path: `DOC_ROOT/<category>/<Name> PRD.md` (see §1 of the rules file).

### Skeleton (keep this order; sections marked *optional* may be omitted)

```markdown
# <Name> PRD

<PRD metadata table from §2>

## ▲ Objective

## ▲ Success Metrics

## ▲ Implementation Assumptions

## ▲ Roadmap

[Note to LLM: IGNORE. This section will be handled separately.]

## ▲ Requirements

## ▲ <Design Choice> Decision Matrix        ← optional, see below

## ▲ Why <Chosen Approach>?                 ← optional

## ▲ <State Diagram | Design Overview Diagram | System Architecture Diagram>

## ◎ Out of Scope
```

### Section requirements

**Objective** (2–4 bullets)
1. What the system is responsible for: which game actions it handles, in both driver control and autonomous.
2. Its behavioral requirements: responsiveness, consistency between runs, precision, and safety.
3. The core architectural choice, in **bold**, with a one-clause reason. Example: "implemented as a **finite state machine (FSM)** so the lift and arm are always commanded from one place with no conflicting outputs."
4. *(Optional)* The physical layout that shapes the design, such as stages, actuators, or how the mechanism is driven.

**Success Metrics** (4–8 rows, `|  **Goal**  |  **Metric**  |`)
- **Goal:** a capability in plain language. Example: "The lift reaches each scoring height on command."
- **Metric:** an observable pass condition that names concrete states, functions, or constants. Example: "In `LEVEL_1`–`LEVEL_5` the lift settles at the matching `LEVEL_N_HEIGHT` and holds it without drifting."
- Cover, where they apply: each major state group or mode, driver-control responsiveness, autonomous usability (non-blocking, how to wait), the background task not blocking other code, safety limits, homing or recovery, and tunability.
- Use no numbers unless they come from the code or the user. Never write "correctly" or "properly" without saying what that means.

**Implementation Assumptions** (2–6 bullets)
- The physical hardware: each motor, sensor, and piston, and what it does.
- Sensor zero conventions, such as where the zero position is and when it is set.
- That motor port sign handles physical reversal, if the code relies on it.
- Preconditions, such as "the control task is started once in `initialize()`".
- Dependencies on other systems.

**Roadmap:** the fixed placeholder line only.

**Requirements** (4–10 rows, `|  **Requirement**  |  **Importance**  |  **Notes**  |`)
- Sort `HIGH` first, then `MEDIUM`, then `LOW`.
- Requirement is a Title Case noun phrase of 6 words or fewer, such as "Finite State Machine", "Dedicated Background Control Task", or "Soft Travel Limits".
- Notes give the concrete identifiers or behavior, such as "`ZERO`, `LOADING`, `LEVEL_1`–`LEVEL_5`, `RAW_*`, `HOMING`, `SCORE`".
- Include one row per major mechanism: state machine, background task, control loop, safety limits, automatic sequences (homing, scoring), driver mapping, autonomous API, and sensor integration.
- Start the Notes cell of a not-yet-built requirement with "Planned —".

**Decision Matrix** (*optional*; include it whenever a real design choice was made, because judges look for it)
- Typical choices: control architecture (FSM vs. direct motor calls), sensor (rotation sensor vs. motor encoder), control law (P vs. PID vs. bang-bang), homing method (current sensing vs. limit switch vs. hard stop), actuator (motor vs. pneumatic).
- Columns: `|  **Option**  |  **How It Works**  |  **Advantages**  |  **Disadvantages**  |`, with an optional `**Use Cases**` column. Include 2–4 realistic options. For multi-line cells, add continuation rows with an empty first cell, like the Driver Control PRD does. **Bold** the chosen option's advantages.
- Follow the table with `**Our Choice: <Option>**` and one paragraph tying the choice to this robot's constraints.
- Present the alternatives as *considered*. Never claim one was built or tested unless the code history or the user says so.

**Why <Chosen Approach>?** (*optional*; 3–5 subsections `### N. <Benefit>`)
Use this pattern for each: `**Problem with <alternative>:**`, then `**<Choice> solution:**`, then `**Real-world impact:**`. Use no invented numbers. A *Quantitative Comparison* table is allowed **only** with measured data the user provides.

**Diagram section**
- For a subsystem with states, use `## ▲ State Diagram`: every state is a node, every transition an arrow labeled with its exact trigger, and each node lists its outputs or targets.
- For a pipeline or loop, use `## ▲ Design Overview Diagram` (flowchart or block diagram).
- For multiple cooperating systems, use `## ▲ System Architecture Diagram`.
- Follow §7 of the rules file exactly. Instructions only, no images.

**Out of Scope** (2–6 bullets)
- Nearby responsibilities this system does **not** handle, and where they are handled ("covered in the Intake documentation", "driven directly from `opcontrol()`").
- Deliberate non-features, and the limits of this document.

### PRD must NOT contain
- Code blocks. Identifiers in backticks are fine.
- Tuning procedures, troubleshooting, usage examples, parameter tables, or file trees. These go in the Implementation doc.
- Invented performance data or superlatives.

## Step 3: Check

Run the §11 Final Checklist from the rules file, plus:
- [ ] Every state, method, and constant named in the PRD exists in the code (Mode A) or is marked "(proposed)" (Mode B).
- [ ] Every HIGH requirement is tested by at least one Success Metric.
- [ ] The diagram's nodes and arrows match the fact sheet's states and transitions exactly.

## Step 4: Report (in chat, not in the doc)

- The path of the file written.
- Every `TODO` left in the doc.
- Code discrepancies found (§6.7).
- Design decisions you inferred rather than found in the code, comments, or commits, so the owner can confirm them.
