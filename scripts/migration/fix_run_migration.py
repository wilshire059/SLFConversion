# Fix the corrupted line in run_migration.py
with open("C:/scripts/SLFConversion/run_migration.py", "r", encoding="utf-8") as f:
    content = f.read()

# Fix the corrupted line
old_line = 'print("POST-MIGRATION: Dodge Montages")nn    with open("C:/scripts/SLFConversion/post_migration_trace.txt", "w") as tf:n'
new_lines = '''print("POST-MIGRATION: Dodge Montages")
    print("=" * 60)'''

if old_line in content:
    content = content.replace(old_line, new_lines)
    with open("C:/scripts/SLFConversion/run_migration.py", "w", encoding="utf-8") as f:
        f.write(content)
    print("Fixed run_migration.py")
else:
    print("Corrupt line not found - may already be fixed")
    print("Searching for POST-MIGRATION section...")
    if "POST-MIGRATION: Dodge Montages" in content:
        idx = content.find("POST-MIGRATION: Dodge Montages")
        print("Found at position", idx)
        print("Context:", repr(content[idx-50:idx+100]))
