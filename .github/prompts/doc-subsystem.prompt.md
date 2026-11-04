---
agent: agent
description: "Document a whole subsystem end to end: PRD, then Implementation, then a consistency pass. Start here for a new subsystem such as Lift."
argument-hint: "Subsystem name, e.g. Lift"
---

# Document a Subsystem (PRD + Implementation)

**Subsystem / feature:** ${input:subsystem:Subsystem name, e.g. Lift}
**Category folder:** ${input:category:Subsystems or Chassis (default Subsystems)}
**Document owner / designer / technical writer:** ${input:owner:Full name(s); one name is used for all three}
**Target release:** ${input:targetRelease:YYYY-MM-DD, or blank for TBD}
**Design brief (optional):** ${input:brief:Anything the code doesn't show: intent, alternatives considered, constraints; or blank}

If any value above is blank or still shows a literal `${input:...}`, apply these defaults: **subsystem** → ask the user (the only thing you must ask), **category** → `Subsystems`, **owner** → `TBD`, **target release** → `TBD`, **brief** → none.

This prompt runs the two document prompts in order and makes the pair consistent. Don't stop to ask questions once the subsystem is known. Put a `> **TODO (owner):**` callout where something is missing, and list every TODO at the end.

---

## Step 1: Load everything once

1. Read `.github/instructions/documentation.instructions.md` in full.
2. Read `.github/prompts/doc-prd.prompt.md` and `.github/prompts/doc-implementation.prompt.md` in full. You will follow both.
3. Read the example docs both prompts list (commands are in §1 of the rules file).

## Step 2: Gather facts once

Run the §10 Fact-Gathering Procedure **once** and build a single fact sheet. Both docs are written from it, so they cannot disagree. If the code doesn't exist yet, write only the PRD (Mode B in the PRD prompt), then stop and say so.

Before writing, settle these shared decisions and use them in both docs:
- **Display name:** `<Name>` for filenames and titles (for example `Lift`). An optional H1 descriptor such as "Lift Mechanism PRD" is fine, but the filenames must match.
- **State grouping:** how you will group the states (for example position states, raw/manual states, automatic sequences). Use the same grouping in both docs.
- **The design choices to justify:** these become the PRD Decision Matrix and the Implementation's Key Design Decisions.
- **The tier** for the Implementation doc (Step 2 of the Implementation prompt).

## Step 3: Write the PRD

Follow `doc-prd.prompt.md` Steps 2–3. Write `DOC_ROOT/<category>/<Name> PRD.md`.

## Step 4: Write the Implementation doc

Follow `doc-implementation.prompt.md` Steps 3–4. Write `DOC_ROOT/<category>/<Name> Implementation.md`.

## Step 5: Consistency pass across the pair

- [ ] Both docs use the same state names, method names, constant names, and state grouping.
- [ ] Every PRD Success Metric Goal appears **word for word** in the Implementation's *Alignment with PRD Goals* table, and every Status there is honest.
- [ ] Every PRD Requirement is covered somewhere in the Implementation's Design Spec, and every `HIGH` one is explained there.
- [ ] Every option in the PRD Decision Matrix that was chosen reappears in the Implementation's *Key Design Decisions*.
- [ ] The PRD diagram and the Implementation diagrams use identical node labels for the same states.
- [ ] The PRD Out of Scope items are not explained in detail in the Implementation, except as cross-references.
- [ ] Both docs pass the §11 Final Checklist in the rules file.

Fix anything that fails, then re-run the pass.

## Step 6: Report (in chat)

```txt
Files written:
  - <path to PRD>
  - <path to Implementation>  (tier: Compact | Full)

TODOs for the owner:
  - <doc> › <section>: <what is needed>

Code discrepancies found (not fixed — documented as actual behavior):
  - <file>:<line> — <issue>

Placeholder constants documented as placeholders:
  - <name> = <value>

Inferred design rationale to confirm:
  - <claim> (doc › section)
```
