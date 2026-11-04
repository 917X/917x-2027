---
applyTo: "docs/**/*.md"
description: "917X engineering documentation rules: location, format, voice, accuracy, and diagrams for PRD and Implementation entries."
---

# 917X Documentation Rules

These rules apply to every engineering documentation entry the team writes: PRDs, Implementation docs, and Overarching Introductions. The prompts in `.github/prompts/doc-*.prompt.md` rely on this file. Read all of it before writing or editing any doc.

The docs are entries in the team's programming engineering notebook. They have two audiences:

1. **Judges and mentors** reading the notebook. They need to see the problem, the alternatives, the chosen design, and why it was chosen.
2. **Future programmers on the team.** They know basic C++ but not this codebase, and they need to understand, maintain, and tune the system from the doc alone.

---

## 1. Location and Naming

**Doc root** (change this one line when the team starts a new robot folder):

```txt
DOC_ROOT = docs/917x/Home/Robot 1/Prog - 1
```

| **Doc kind**              | **Path**                                                           | **H1 title**                                  |
|:--------------------------|:-------------------------------------------------------------------|:----------------------------------------------|
| Subsystem PRD             | `DOC_ROOT/Subsystems/<Name> PRD.md`                                | `# <Name> PRD` (may add a descriptor, e.g. `# Lift Mechanism PRD`) |
| Subsystem Implementation  | `DOC_ROOT/Subsystems/<Name> Implementation.md`                     | `# <Name> Implementation`                     |
| Chassis feature PRD       | `DOC_ROOT/Chassis/<Name> PRD.md`                                   | `# <Name> PRD`                                |
| Chassis feature Impl.     | `DOC_ROOT/Chassis/<Name> Implementation.md`                        | `# <Name> Implementation`                     |
| Category overview         | `DOC_ROOT/<Category>/Overarching <Category> PRD Introduction.md`   | `# Overarching <Category> PRD Introduction`   |

- `<Name>` is Title Case with spaces, no underscores or dashes: `Lift`, `Slew Rate Limiter`, `Point-to-Point Navigation` (keep a hyphen only when it belongs to the name itself).
- A PRD and its Implementation doc use the **same** `<Name>`, so the pair sorts together.
- An optional parenthetical subtitle may follow the H1 title: `# Odometry PRD (Absolute Position Tracking)`.
- Create the folders if they do not exist. Never write docs anywhere else.
- Do **not** add breadcrumb navigation lines such as `[Robot 1](../../../Robot%201.md) > ...`. They were removed on purpose.
- Images are added later by a human into `docs/917x/attachments/`. Never create or reference image files yourself.

### Reference examples (format only)

The existing docs live on other branches. Read them with `git show` (run `git fetch origin` first if a ref is missing):

```bash
git show "origin/lever:docs/917x/Home/Robot 1/Prog - 1/Subsystems/Intake PRD.md"
git show "origin/lever:docs/917x/Home/Robot 1/Prog - 1/Subsystems/Intake Implementation.md"
git show "origin/lever:docs/917x/Home/Robot 1/Prog - 1/Chassis/PID Controller PRD.md"
git show "origin/lever:docs/917x/Home/Robot 1/Prog - 1/Chassis/PID Controller Implementation.md"
git show "origin/lever:docs/917x/Home/Robot 1/Prog - 1/Chassis/Slew Rate Limiter Implementation.md"
git show "origin/lever:docs/917x/Home/Robot 1/Prog - 1/Chassis/Driver Control PRD.md"
git show "origin/lever:docs/917x/Home/Robot 1/Prog - 1/Chassis/Overarching Drive Chassis PRD Introduction.md"
git show "origin/sensor-fusion:docs/917x/Home/Robot 1/Prog - 1/Chassis/IMU Fusion Implementation.md"
```

| **Example**                          | **Use it as the model for**                                                        |
|:-------------------------------------|:-----------------------------------------------------------------------------------|
| Intake PRD / Implementation          | A state-machine subsystem. This is the closest match for any new mechanism.        |
| PID Controller PRD                   | Decision matrix, the "Why X is superior" argument, flowchart instructions          |
| PID Controller Implementation        | Math, Tuning Methodology, Troubleshooting, Key Takeaways for a control system      |
| Slew Rate Limiter Implementation     | A compact feature doc: math, two code excerpts, Key API, Execution Flowchart       |
| Driver Control PRD                   | Decision matrix with alternatives and the **Our Choice** paragraph                 |
| IMU Fusion Implementation            | Step-by-step derivations, Parameter Reference table, Thread Safety and Task Timing |
| Overarching Drive Chassis PRD Intro  | A category overview                                                                |

These examples describe **last season's code**. Copy their structure and tone, never their facts. Section 9 lists flaws in them that you must not repeat.

---

## 2. Metadata Table

Every doc starts with the H1 title, a blank line, then this table. Keep the empty header row and the left-aligned divider exactly as shown.

PRD and Overarching Introduction (no Target release):

```markdown
|                       |            |
|:----------------------|:-----------|
| **Document status**   | DRAFT      |
| **Document owner**    | <Name>     |
| **Designer**          | <Name>     |
| **Technical writers** | <Name>     |
```

Implementation:

```markdown
|                       |            |
|:----------------------|:-----------|
| **Target release**    | YYYY-MM-DD |
| **Document status**   | DRAFT      |
| **Document owner**    | <Name>     |
| **Designer**          | <Name>     |
| **Technical writers** | <Name>     |
```

- **Document status:** new docs are always `DRAFT`. Only use `PROD` when the user explicitly says the doc is reviewed and final.
- **Names:** use the names the user gives. If they give only one name, use it in all three rows. Never invent a name. If you have none, write `TBD` and report it.
- **Target release:** the date the subsystem should be competition-ready, if the user gives it. Otherwise write `TBD`. Do not copy dates from other docs.

---

## 3. Headings and Section Markers

- `#` is used once, for the title.
- Every top-level body section is `## ▲ <Section Name>`. The **only** exception is `## ◎ Out of Scope`, which uses `◎`.
- Use the exact section names from the prompt skeletons, in Title Case: `Success Metrics`, not `Success metrics`.
- `## ▲ In This Document:` keeps its trailing colon. It appears only in Implementation docs.
- `###` marks subsections and `####` marks sub-subsections. Never go deeper than `####`.
- Implementation Explainer subsections use Roman numerals with a period, such as `### Section I. Core Control Loop`.
- Step sequences use `### Step N: <Title>` (derivations and tuning). Numbered decisions or examples use `### N. <Title>` or `### Example N: <Title>`.

---

## 4. Formatting

### Tables

- Header cells are bold with two spaces of padding on each side: `|  **Goal**  |  **Metric**  |`.
- The divider row is left-aligned: `|:---|:---|`. Pad columns so the raw markdown lines up when practical.
- Importance values are uppercase: `HIGH`, `MEDIUM`, `LOW`.
- Status values in alignment tables are `✅ Met`, `⚠️ Partial`, and `❌ Not Met`.

### Prose

- Bullets are complete sentences that start with a capital letter. Keep them to 1–3 sentences.
- Put every code identifier in backticks: class, function, enum value, variable, constant, file, port constant.
- Use **bold** for a concept the reader must remember, never for decoration.
- Write function references with parentheses, as in `liftControl()`, and enum values the way the code spells them (`LEVEL_3`, or `Lift::LEVEL_3` in code).

### Code blocks

- Use ` ```cpp ` for C++, ` ```txt ` for file trees and plain pseudo-steps, and ` ```bash ` for shell.
- Make the first line of every C++ excerpt a comment naming its source file, for example `// src/subsystem/lift.cpp`.
- Excerpts are **copied from the current source**, never retyped from memory. You may:
  - trim irrelevant lines and replace them with `// ...`;
  - add comments that explain *why* a line exists. This is the main value of an excerpt.
- You may **not** rename identifiers, change values, "clean up" the logic, or show code that does not exist. A pseudo-code walkthrough is allowed only if the lead-in sentence calls it a **simplified** view.
- Keep each excerpt under about 60 lines. Split a longer function across Explainer steps.

### File trees

Use a ` ```txt ` block with `├──`, `└──`, and `│` connectors, and a `# comment` after each file saying what it contains. List only files that matter to this doc, and use the real paths on the current branch.

### Math

- Use math only where the code has real math, such as a control law, a filter, geometry, or a unit conversion.
- Inline math uses `$...$`. Display math uses `$$` on its own line before and after the expression.
- Follow every equation with a **Where:** bullet list that defines each symbol, gives its unit, and maps it to the code variable (for example, $K_p$ is `kP`).
- Follow the definitions with a plain-language **Intuition:** or **Why this formula?** paragraph.

### Units and conventions (VEX V5 / PROS)

- Always state units: inches, degrees, ms, mA, RPM, Hz.
- `pros::Motor::move()` takes **motor power from −127 to 127**. Call it "power" unless the code uses `move_voltage()` (millivolts).
- `pros::Rotation::get_position()` returns **centidegrees**. If the code divides by 100, say so where the value is introduced.
- A negative port number reverses that motor or sensor. Mention this wherever ports are listed.
- Loop rate: a `pros::delay(10)` loop runs at **100 Hz**. Give both the period and the frequency.

---

## 5. Voice and Content

- Use "we" and "our" for team decisions ("We chose a state machine because..."). Describe code in the present tense ("`liftControl()` reads the rotation sensor every 10 ms").
- **Explain why, not just what.** Every design choice needs a reason. For justification, use the pattern **Problem → Solution → Impact**: what goes wrong without it, what the design does, and why that matters in a match.
- Define a term the first time it appears: FSM, PID, kP, P-controller, stall, soft limit, centidegrees.
- Cross-reference other entries by name instead of re-explaining them: "(See PID Controller Entry)", "covered in the Intake documentation", "documented separately".
- No marketing language or unverifiable superlatives. Banned examples: "world-class", "seamlessly", "revolutionary", "the first ... in VEX", "dramatically", "clearly superior" (unless it is a section title that the argument underneath then supports).
- No thinking aloud. Write each sentence as a final statement, not a half-finished thought.
- Describe the code as it **is**. Planned features belong only in PRD Requirements (Notes column: "Planned") and show as `❌ Not Met` or `⚠️ Partial` in the Implementation's Alignment table.
- Don't document anything the code marks as last season's or reference-only (for example, objects `config.hpp` labels "Last season (Push Back) - reference only").

---

## 6. Accuracy: The Code Is the Source of Truth

These rules override everything else in this file.

1. **Read before you cite.** Open and read every file whose code, path, or values you mention.
2. **Match exactly.** Class names, method signatures, enum values and their order, member names, default values, port constants, and loop delays must match the source character for character.
3. **Paths must exist.** Every path in a File Structure tree must appear in `git ls-files` on the current branch, except doc paths you are creating.
4. **Placeholders stay placeholders.** If the code marks a value as `PLACEHOLDER`, `tune on the robot`, or similar, the doc says "(placeholder — tune on robot)" next to it. Never present a placeholder as a tuned value.
5. **No invented data.** Never invent measurements, test results, success percentages, timings, competition results, or dates. Code comments that state measurements (for example, "Measured at -45: loose is 50-105 mA") may be quoted with that source. If a made-up example makes something clearer, label it in italics: *Illustrative example, not measured data.*
6. **Sourced rationale.** Design reasons come from code comments, commit messages (`git log --follow -p -- <file>`), the user's brief, or standard engineering reasoning (for example, why a state machine avoids conflicting motor commands). Don't invent history such as "after testing at States we noticed...".
7. **Surface discrepancies, don't hide them.** If the code contradicts itself (a reference to a symbol that doesn't exist, dead code, a comment that disagrees with the logic), document the **actual** behavior and list the discrepancy in your final chat report. Don't quietly "fix" it in the doc.
8. **Mark missing facts.** When something can't be determined, put a TODO callout in the doc at that spot and list it in the report:

   ```markdown
   > **TODO (owner):** Confirm the gear ratio between `liftMotor` and the lift axle.
   ```

---

## 7. Diagrams (Instructions Only)

Diagrams are drawn later by a human. **Never** produce an image, Mermaid, PlantUML, or ASCII-art diagram. Write instructions detailed enough that someone who has never seen the code could draw it.

**Section-level diagram** (its own `## ▲` section):

```markdown
## ▲ State Diagram

**Diagram Instructions:**
Create a <diagram type> with <N> nodes:

- **`STATE_A`** — <position, e.g. center/default>
- ...

Draw directed arrows for <which transitions>. Label each arrow with <the exact trigger, e.g. "lift.set(LEVEL_1)">.
Inside each node, list <outputs/targets>.

Use color coding: <color> for <group>, ...
Add annotations: "<short note>", ...
```

Use `**Flowchart Instructions:**` instead of `**Diagram Instructions:**` for flowcharts.

**Inline diagram** (inside a Design Spec subsection):

```markdown
[Note: Insert diagram here — **"<Diagram Title>"**. <Full drawing instructions in one paragraph.>]
```

Every diagram instruction must specify:

1. **Type:** state diagram, flowchart, block diagram, sequence diagram, timing diagram, or plot.
2. **Nodes:** each one with its exact label, using real identifiers from the code.
3. **Edges:** each arrow with its label (the trigger, condition, or data).
4. **Layout:** where key nodes sit (top, center, left lane, and so on).
5. **Color coding:** one consistent legend per doc.
6. **Annotations:** the 1–3 insights the diagram must communicate.

Diagram types that fit common situations:

| **Situation**                                   | **Diagram**                                                           |
|:------------------------------------------------|:----------------------------------------------------------------------|
| Subsystem with states                           | State diagram: nodes are states, arrows are transitions               |
| Per-tick control logic                          | Flowchart of one loop iteration                                       |
| Multi-phase automatic sequence (homing, scoring)| Flowchart or timing diagram of the phases and their exit conditions   |
| User code ↔ background task interaction         | Sequence diagram with lanes                                           |
| Signal or data pipeline                         | Left-to-right block diagram                                           |
| Speed or position profile                       | Plot with labeled axes and the key points marked                      |

---

## 8. Fixed Placeholders

These sections are filled in later by a human. Include them where the skeleton calls for them, and write **exactly** this line as the section body, unchanged:

```markdown
[Note to LLM: IGNORE. This section will be handled separately.]
```

- PRD: `## ▲ Roadmap`
- Implementation: `## ▲ Tests & Performance Analysis`, `## ▲ Issues`, `## ▲ Version History`

Never fill these sections in, and never remove them.

---

## 9. Known Flaws in the Old Docs — Do Not Repeat

| **Flaw**                                                                                                         | **Rule it breaks**   |
|:-----------------------------------------------------------------------------------------------------------------|:---------------------|
| PID PRD "Quantitative Comparison" and PID Impl "Performance Characteristics" tables use numbers nobody measured  | §6.5 No invented data |
| Drive Impl file tree shows `include/lib/...` paths that never existed; Intake docs cite `devices.cpp` / `subsystems/` (now `config.cpp` / `subsystem/`) | §6.3 Paths must exist |
| Sensor Fusion PRD claims "the first multi-sensor position fusion of this kind implemented in competitive VEX V5 robotics" | §5 No superlatives   |
| Point-to-Point PRD (518 lines) repeats the Implementation's Troubleshooting, Usage, and Parameters sections     | PRD scope (what/why only) |
| IMU Fusion usage example contains a half-finished thought in a code comment                                     | §5 No thinking aloud |
| 9 of the 11 Implementation docs reuse `Target release 2025-11-01`, including docs written months later (Feb–Mar 2026) | §2 Target release    |
| Mixed `Success metrics`/`Success Metrics`, `Section I.`/`Section I:`, and three different diagram-placeholder styles | §3, §7 Consistency   |

---

## 10. Fact-Gathering Procedure

Run this before writing about any code. The current layout (2026-27 robot, `main`) is:

```txt
include/config.hpp        # extern declarations for every device and subsystem object
src/config.cpp            # port constants (constexpr) and object construction
include/subsystem/*.hpp   # subsystem class declarations
src/subsystem/*.cpp       # subsystem implementations
src/main.cpp              # initialize() starts the background tasks; opcontrol() maps the buttons
src/autons.cpp            # autonomous routines
include/EZ-Template/, src/EZ-Template/   # chassis library (ez::Drive, ez::Piston, ...)
```

1. **Locate the code:** `git ls-files | grep -i <name>`, then read the header and source **in full**.
2. **Hardware and construction:** `grep -n "<object>\|<CONSTANT>" include/config.hpp src/config.cpp`. Record every port constant, its sign (reversed?), the sensor types, the pneumatics (ADI port letters), and any `PLACEHOLDER` comments.
3. **Task launch and initial state:** `grep -n "<object>" src/main.cpp`. Find where `initialize()` starts the task and sets the initial state.
4. **Driver controls:** read `opcontrol()` in `src/main.cpp`, including any button-map comment block. Record each button constant, its condition (press, new press, hold, release), and the call it makes.
5. **Autonomous usage:** `grep -n "<object>\." src/autons.cpp`. Check each call against the current API, and flag any call that would not compile.
6. **Interactions:** find the other devices or subsystems it reads or commands (for example, a shared piston), and anything else that commands its hardware.
7. **History and rationale:** run `git log --oneline -- <files>`, then `git log -p -- <files>` for commit messages and comment changes that explain design choices.
8. **Build a fact sheet** for yourself (do not paste it into the doc):
   - Class name, constructor signature, and the hardware references it holds
   - Every enum, with all values in declaration order, including nested or sub-phase enums
   - Every public method: signature, behavior in one line, guards (when the call is ignored), and entry actions (what it resets or latches)
   - Per-state behavior: the target or output for **each** actuator
   - Transitions: external ones (which call), automatic ones (which condition inside the loop), and guards
   - Control law for each actuator: formula, clamp, gain names, units
   - Safety logic: soft limits, timeouts, current or velocity thresholds
   - Tunable constants: name, default, unit, placeholder or tuned, and what increasing it does
   - Loop period and frequency, and where the task is launched
   - Driver button map and autonomous usage patterns
   - Discrepancies and open questions

---

## 11. Final Checklist (run before reporting done)

- [ ] File is at the correct path, with the correct filename and H1.
- [ ] Metadata table is exact. Status is `DRAFT` unless told otherwise. No invented names or dates.
- [ ] Every `##` heading uses `▲`, except `◎ Out of Scope`. Section names and order match the skeleton.
- [ ] Every identifier, value, and path was checked against the current source.
- [ ] Placeholder constants are labeled as placeholders.
- [ ] No invented measurements, superlatives, or unsourced history.
- [ ] Every equation has a **Where:** list with units.
- [ ] Every diagram is instructions only, with type, nodes, edges, layout, colors, and annotations.
- [ ] Fixed placeholder sections are present with the exact note text.
- [ ] PRD and Implementation agree on names, states, and requirements, and the Alignment table copies the PRD's Goals word for word.
- [ ] Final chat report lists the files written, every TODO, and every code discrepancy found.
