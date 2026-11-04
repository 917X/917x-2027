---
agent: agent
description: "Write the Overarching <Category> PRD Introduction that opens a notebook category (e.g. Subsystems) and indexes its PRD/Implementation entries."
argument-hint: "Category name, e.g. Subsystems"
---

# Write an Overarching Category Introduction

**Category:** ${input:category:Category folder name, e.g. Subsystems or Chassis}
**Document owner / designer / technical writer:** ${input:owner:Full name(s); one name is used for all three}

If any value above is blank or still shows a literal `${input:...}`: **category** → ask the user, **owner** → `TBD`.

The overview is the first page a reader sees for a category. It explains what the category as a whole is responsible for and lists the entries that follow. It is short (30–70 lines) and covers **no** component in detail, because each entry does that.

---

## Step 0: Load the rules and the example

1. Read `.github/instructions/documentation.instructions.md` in full.
2. Read the example: `git show "origin/lever:docs/917x/Home/Robot 1/Prog - 1/Chassis/Overarching Drive Chassis PRD Introduction.md"`.

## Step 1: Gather facts

- List the existing entries: `ls "DOC_ROOT/<category>/"`.
- List the code that belongs to the category. For `Subsystems`: `include/subsystem/*.hpp`, `src/subsystem/*.cpp`, the objects declared in `include/config.hpp`, and the tasks launched in `initialize()` in `src/main.cpp`.
- For each component, note in one line what it does and whether it has a PRD, an Implementation doc, both, or neither.
- Note the architecture the components share, for example "every subsystem is a class with a state machine running in its own PROS task, constructed in `config.cpp`".

## Step 2: Write the doc

Path: `DOC_ROOT/<category>/Overarching <Category> PRD Introduction.md`

```markdown
# Overarching <Category> PRD Introduction

<PRD metadata table from §2 — no Target release>

## ▲ Objective

## ▲ Success Metrics

## ▲ Implementation Assumptions

## ▲ Roadmap

[Note to LLM: IGNORE. This section will be handled separately.]

## ▲ Requirements

## ▲ Design Overview Diagram

## ◎ Out of Scope
```

### Section requirements

- **Objective** (2–3 bullets): what this category is responsible for during the match, and why it matters (for example, the subsystems are how the robot scores). Add one bullet on the shared architectural pattern and why we standardize on it.
- **Success Metrics** (3–6 rows, `Goal | Metric`): goals that apply at the category level, such as every mechanism being commandable from driver control and autonomous, none blocking the main program, each implemented as its own class, and consistent behavior between runs.
- **Implementation Assumptions** (2–4 bullets): the hardware is built and wired as listed in `config.cpp`, the background tasks are launched in `initialize()`, and so on.
- **Requirements** (one row per component, `Requirement | Importance | Notes`): the Requirement is the component name ("Lift Mechanism", "Intake Mechanism"), Importance is its match priority, and Notes says what it's for. Add "(See <Name> Entry)" if the doc exists, or "Entry planned" if it doesn't.
- **Design Overview Diagram:** `**Diagram Instructions:**` for a block diagram. Put the controller and autonomous routines on the left, the subsystem objects in the middle (each labeled with its class and task), and the physical hardware (motors, sensors, pistons, with port constants) on the right. Show the interactions between subsystems (for example, a shared piston). Follow §7 of the rules file.
- **Out of Scope:** first, the line "This section serves **merely** as a broad overview of our <category> implementation. Specifics of each component are detailed in the subsequent entries of this notebook." Then list the neighboring categories that are covered elsewhere (for example, the chassis is covered in the Chassis section).

## Step 3: Check and report

Run the §11 Final Checklist from the rules file. Then report in chat: the file path, the component entries that are still missing (candidates for `doc-subsystem.prompt.md`), and any TODOs.
