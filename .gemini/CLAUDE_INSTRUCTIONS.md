# Claude Instructions for Gemini Peer Review System

## MANDATORY: Use Gemini in These Situations

### 1. After Context Compaction
```bash
cd C:/scripts/slfconversion && cat .gemini/GEMINI_SYSTEM.md .gemini/MIGRATION_STATUS.md | gemini -m gemini-2.5-pro "Claude's context was just compacted. Summarize: 1) What was being worked on 2) Current state 3) Next steps. Be specific."
```

### 2. Before Starting Any Code Change
```bash
cat .gemini/GEMINI_SYSTEM.md | gemini -m gemini-2.5-pro "I'm about to [DESCRIBE CHANGE]. Review this approach: [DETAILS]. What could go wrong? Is this the right fix?"
```

### 3. Before Modifying C++ Code
```bash
cat [FILE_TO_MODIFY] | gemini -m gemini-2.5-pro "$(cat .gemini/GEMINI_SYSTEM.md) Review this file. I plan to: [CHANGE]. Check for issues."
```

### 4. When Stuck on a Problem
```bash
cat .gemini/GEMINI_SYSTEM.md .gemini/MIGRATION_STATUS.md | gemini -m gemini-2.5-pro "I'm stuck on: [PROBLEM]. Current state: [DETAILS]. What am I missing?"
```

### 5. After Making Changes - Validation
```bash
cat [MODIFIED_FILE] | gemini -m gemini-2.5-pro "$(cat .gemini/GEMINI_SYSTEM.md) Validate this change. Does it follow UE5 best practices? Any bugs?"
```

## Update Migration Status

After completing any significant work:
```bash
# Add to status tracker
echo "## Update $(date)
- Completed: [what]
- Status: [result]
- Next: [what's next]" >> .gemini/MIGRATION_STATUS.md
```

## Quick Reference Commands

```bash
# Recovery after compaction
cat .gemini/GEMINI_SYSTEM.md .gemini/MIGRATION_STATUS.md | gemini -m gemini-2.5-pro "Recovery mode. Summarize current state and next steps."

# Code review
cat [file] | gemini -m gemini-2.5-pro "Review for UE5 best practices and bugs."

# Approach validation
echo "[describe approach]" | gemini -m gemini-2.5-pro "Is this the right approach for a BP to C++ migration? What's missing?"

# Check documentation
gemini "Using the Soulslike Framework docs at https://soulslike-framework.isik.vip/, explain [topic]"
```

## What to Include in Gemini Prompts

1. **Always include**: System prompt (.gemini/GEMINI_SYSTEM.md)
2. **For debugging**: Migration status + specific error/symptom
3. **For code review**: The actual code file
4. **For approach review**: Current state + proposed change + goal

## Gemini Response Handling

- If Gemini finds issues → Address them before proceeding
- If Gemini suggests alternatives → Consider seriously, discuss with user if significant
- If Gemini approves → Proceed but stay vigilant
- If Gemini is unsure → Gather more info, re-query with details
