#!/bin/bash
# Gemini Peer Review Helper for SLFConversion
# Usage: ./gemini-review.sh [command] [args]

GEMINI_DIR="C:/scripts/slfconversion/.gemini"
SYSTEM_PROMPT="$GEMINI_DIR/GEMINI_SYSTEM.md"
STATUS_FILE="$GEMINI_DIR/MIGRATION_STATUS.md"
SESSION_LOG="$GEMINI_DIR/session_log.txt"
# Use gemini-2.5-pro to avoid rate limits on gemini-3-pro-preview
GEMINI_MODEL="gemini-2.5-pro"

# Ensure session log exists
touch "$SESSION_LOG"

case "$1" in
  "review-code")
    # Review a code file
    FILE="$2"
    if [ -f "$FILE" ]; then
      echo "Reviewing: $FILE"
      cat "$SYSTEM_PROMPT" "$FILE" | gemini -m $GEMINI_MODEL "Review this code for issues. Be critical. Check for UE5 best practices, memory safety, and correctness."
    else
      echo "File not found: $FILE"
    fi
    ;;

  "review-change")
    # Review a proposed change
    echo "Describe the proposed change, then Ctrl+D:"
    CHANGE=$(cat)
    echo -e "System context:\n$(cat $SYSTEM_PROMPT)\n\nProposed change:\n$CHANGE" | gemini -m $GEMINI_MODEL "Critically review this proposed change. What could go wrong? What's missing? Is this the right approach?"
    ;;

  "recover")
    # Help recover after context compaction
    echo "Recovery mode - reading status and recent session..."
    cat "$SYSTEM_PROMPT" "$STATUS_FILE" | gemini -m $GEMINI_MODEL "Claude's context was compacted. Based on the migration status, summarize: 1) What was being worked on 2) Current blockers 3) Recommended next steps"
    ;;

  "validate")
    # Validate current state
    echo "Running validation..."
    cat "$STATUS_FILE" | gemini -m $GEMINI_MODEL "Review this migration status. What items need attention? What might be missing from the tracker?"
    ;;

  "update-status")
    # Update status with new info
    echo "Enter status update (Ctrl+D when done):"
    UPDATE=$(cat)
    echo -e "\n---\n## Update $(date)\n$UPDATE" >> "$STATUS_FILE"
    echo "Status updated."
    ;;

  "ask")
    # General question with project context
    shift
    QUESTION="$*"
    cat "$SYSTEM_PROMPT" "$STATUS_FILE" | gemini -m $GEMINI_MODEL "$QUESTION"
    ;;

  *)
    echo "Gemini Peer Review Helper"
    echo "Commands:"
    echo "  review-code <file>  - Review a code file"
    echo "  review-change       - Review a proposed change (interactive)"
    echo "  recover             - Help recover after context compaction"
    echo "  validate            - Validate current migration status"
    echo "  update-status       - Add update to status tracker"
    echo "  ask <question>      - Ask Gemini with project context"
    ;;
esac
