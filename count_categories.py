
import collections
import re

yaml_path = r"c:\scripts\SLFConversion\blueprints_inventory_comprehensive.yaml"

counts = collections.defaultdict(int)
with open(yaml_path, 'r', encoding='utf-8') as f:
    for line in f:
        if line.strip().startswith("- name:"):
            # We need to look ahead or behind for category, but easier to just grep "category:" lines inside items
            pass
        if line.strip().startswith("category:"):
            cat = line.strip().split(": ")[1]
            counts[cat] += 1

print("Counts by category:")
for cat, count in sorted(counts.items(), key=lambda x: -x[1]):
    print(f"{cat}: {count}")
