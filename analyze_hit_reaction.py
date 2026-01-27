import json

with open('C:/scripts/slfconversion/Exports/BlueprintDNA_v2/Component/AC_AI_CombatManager.json', 'r') as f:
    data = json.load(f)

# Check Logic section
logic = data.get('Logic', {})
print(f"Logic keys: {list(logic.keys())}")

# Find HandleHitReaction in Logic
for graph in logic.get('FunctionGraphs', []):
    if graph.get('GraphName') == 'HandleHitReaction':
        print(f"\n=== HandleHitReaction Graph Nodes ===")
        for node in graph.get('Nodes', []):
            title = node.get('NodeTitle', '')
            if title:
                print(f"  - {title}")
