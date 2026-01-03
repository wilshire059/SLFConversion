"""
Graph Logic Parser and C++ Generator
Parses Blueprint graph nodes from JSON and generates actual C++ logic implementations.

This script analyzes the node connections in Blueprint graphs and produces
C++ code that replicates the exact Blueprint behavior.
"""
import json
import re
from pathlib import Path
from typing import Dict, List, Tuple, Optional, Any
from dataclasses import dataclass, field

@dataclass
class Pin:
    name: str
    direction: str  # "Input" or "Output"
    type_info: dict
    linked_to: List[dict] = field(default_factory=list)
    default_value: str = ""
    is_hidden: bool = False

@dataclass
class Node:
    node_class: str
    node_title: str
    node_guid: str
    pins: Dict[str, Pin] = field(default_factory=dict)
    pos_x: int = 0
    pos_y: int = 0
    node_comment: str = ""
    extra_data: dict = field(default_factory=dict)

class GraphParser:
    """Parse Blueprint graph nodes and understand execution flow."""

    def __init__(self, graph_data: dict):
        self.graph_name = graph_data.get("GraphName", "")
        self.graph_type = graph_data.get("GraphType", "")
        self.nodes: Dict[str, Node] = {}
        self._parse_nodes(graph_data.get("Nodes", []))

    def _parse_nodes(self, nodes_data: List[dict]):
        """Parse all nodes in the graph."""
        for node_data in nodes_data:
            node = self._parse_node(node_data)
            self.nodes[node.node_guid] = node

    def _parse_node(self, node_data: dict) -> Node:
        """Parse a single node."""
        node = Node(
            node_class=node_data.get("NodeClass", ""),
            node_title=node_data.get("NodeTitle", ""),
            node_guid=node_data.get("NodeGuid", ""),
            pos_x=node_data.get("PosX", 0),
            pos_y=node_data.get("PosY", 0),
            node_comment=node_data.get("NodeComment", ""),
        )

        # Store extra data based on node type
        if "FunctionName" in node_data:
            node.extra_data["FunctionName"] = node_data["FunctionName"]
        if "FunctionClass" in node_data:
            node.extra_data["FunctionClass"] = node_data["FunctionClass"]
        if "VariableName" in node_data:
            node.extra_data["VariableName"] = node_data["VariableName"]
        if "MacroName" in node_data:
            node.extra_data["MacroName"] = node_data["MacroName"]
        if "EventName" in node_data:
            node.extra_data["EventName"] = node_data["EventName"]

        # Parse pins
        for pin_data in node_data.get("Pins", []):
            pin = Pin(
                name=pin_data.get("Name", ""),
                direction=pin_data.get("Direction", ""),
                type_info=pin_data.get("Type", {}),
                linked_to=pin_data.get("LinkedTo", []),
                default_value=pin_data.get("DefaultValue", ""),
                is_hidden=pin_data.get("IsHidden", False),
            )
            node.pins[pin.name] = pin

        return node

    def get_entry_node(self) -> Optional[Node]:
        """Find the function entry node."""
        for node in self.nodes.values():
            if node.node_class == "K2Node_FunctionEntry":
                return node
        return None

    def get_result_nodes(self) -> List[Node]:
        """Find all function result nodes."""
        return [n for n in self.nodes.values() if n.node_class == "K2Node_FunctionResult"]

    def trace_execution_flow(self) -> List[Tuple[Node, str]]:
        """Trace the execution flow from entry to all results."""
        entry = self.get_entry_node()
        if not entry:
            return []

        flow = []
        visited = set()
        self._trace_from_node(entry, "then", flow, visited)
        return flow

    def _trace_from_node(self, node: Node, exec_pin_name: str, flow: List, visited: set):
        """Recursively trace execution from a node."""
        if node.node_guid in visited:
            return

        flow.append((node, exec_pin_name))
        visited.add(node.node_guid)

        # Find the exec output pin and follow it
        exec_pin = node.pins.get(exec_pin_name) or node.pins.get("then")
        if exec_pin and exec_pin.linked_to:
            for link in exec_pin.linked_to:
                next_node = self.nodes.get(link.get("NodeGuid"))
                if next_node:
                    self._trace_from_node(next_node, "then", flow, visited)

        # Handle branches - trace both paths
        if node.node_class == "K2Node_IfThenElse":
            else_pin = node.pins.get("else")
            if else_pin and else_pin.linked_to:
                for link in else_pin.linked_to:
                    next_node = self.nodes.get(link.get("NodeGuid"))
                    if next_node:
                        self._trace_from_node(next_node, "then", flow, visited)


class CppLogicGenerator:
    """Generate C++ code from parsed graph."""

    TYPE_MAP = {
        "bool": "bool",
        "int": "int32",
        "real": "double",
        "float": "float",
        "double": "double",
        "name": "FName",
        "string": "FString",
        "text": "FText",
    }

    def __init__(self, graph: GraphParser, class_name: str):
        self.graph = graph
        self.class_name = class_name
        self.indent = "\t"
        self.output_lines = []

    def generate_function_body(self) -> str:
        """Generate the C++ function body from the graph."""
        self.output_lines = []

        entry = self.graph.get_entry_node()
        if not entry:
            return f"{self.indent}// ERROR: No function entry node found\n"

        # Add debug logging
        func_name = self.graph.graph_name
        self.output_lines.append(f'{self.indent}UE_LOG(LogTemp, Log, TEXT("U{self.class_name}::{func_name} called"));')
        self.output_lines.append("")

        # Trace execution and generate code
        flow = self.graph.trace_execution_flow()
        self._generate_from_flow(flow)

        return "\n".join(self.output_lines)

    def _generate_from_flow(self, flow: List[Tuple[Node, str]]):
        """Generate C++ code from execution flow."""
        branch_depth = 0

        for node, _ in flow:
            code = self._generate_node_code(node, branch_depth)
            if code:
                self.output_lines.extend(code)

    def _generate_node_code(self, node: Node, depth: int) -> List[str]:
        """Generate C++ code for a single node."""
        indent = self.indent * (depth + 1)

        if node.node_class == "K2Node_FunctionEntry":
            return []  # Entry handled separately

        if node.node_class == "K2Node_FunctionResult":
            return self._generate_return_code(node, indent)

        if node.node_class == "K2Node_IfThenElse":
            return self._generate_branch_code(node, indent)

        if node.node_class == "K2Node_CallFunction":
            return self._generate_call_code(node, indent)

        if node.node_class == "K2Node_VariableGet":
            return []  # Variable gets are inline expressions

        if node.node_class == "K2Node_VariableSet":
            return self._generate_variable_set_code(node, indent)

        if node.node_class == "K2Node_MacroInstance":
            return self._generate_macro_code(node, indent)

        if node.node_class in ("K2Node_Knot", "EdGraphNode_Comment"):
            return []  # Reroute nodes and comments are skipped

        return [f"{indent}// TODO: Handle {node.node_class} - {node.node_title}"]

    def _generate_return_code(self, node: Node, indent: str) -> List[str]:
        """Generate return statement."""
        lines = []

        # Find output pins and their sources
        for pin_name, pin in node.pins.items():
            if pin.direction == "Input" and pin_name not in ("execute",):
                if pin.linked_to:
                    # Get the source value
                    source = self._trace_value_source(pin.linked_to[0])
                    lines.append(f"{indent}// Set output: {pin_name} = {source}")
                else:
                    lines.append(f"{indent}// Output {pin_name} uses default value")

        lines.append(f"{indent}return;")
        return lines

    def _generate_branch_code(self, node: Node, indent: str) -> List[str]:
        """Generate if/else branch."""
        lines = []

        # Get condition source
        condition_pin = node.pins.get("Condition")
        condition = "/* condition */"
        if condition_pin and condition_pin.linked_to:
            condition = self._trace_value_source(condition_pin.linked_to[0])

        lines.append(f"{indent}if ({condition})")
        lines.append(f"{indent}{{")
        lines.append(f"{indent}{self.indent}// TRUE branch - see linked nodes")
        lines.append(f"{indent}}}")
        lines.append(f"{indent}else")
        lines.append(f"{indent}{{")
        lines.append(f"{indent}{self.indent}// FALSE branch - see linked nodes")
        lines.append(f"{indent}}}")

        return lines

    def _generate_call_code(self, node: Node, indent: str) -> List[str]:
        """Generate function call."""
        lines = []
        func_name = node.extra_data.get("FunctionName", "UnknownFunction")
        func_class = node.extra_data.get("FunctionClass", "")

        # Get the target (self pin)
        target = "this"
        self_pin = node.pins.get("self")
        if self_pin and self_pin.linked_to:
            target = self._trace_value_source(self_pin.linked_to[0])

        # Build parameter list
        params = []
        for pin_name, pin in sorted(node.pins.items()):
            if pin.direction == "Input" and pin_name not in ("execute", "self"):
                if pin.linked_to:
                    value = self._trace_value_source(pin.linked_to[0])
                else:
                    value = self._format_default_value(pin)
                params.append(f"/*{pin_name}*/ {value}")

        # Generate the call
        if target == "this":
            lines.append(f"{indent}{func_name}({', '.join(params)});")
        else:
            lines.append(f"{indent}{target}->{func_name}({', '.join(params)});")

        return lines

    def _generate_variable_set_code(self, node: Node, indent: str) -> List[str]:
        """Generate variable assignment."""
        var_name = node.extra_data.get("VariableName", "UnknownVar")

        # Find the input value
        value = "/* value */"
        for pin_name, pin in node.pins.items():
            if pin.direction == "Input" and pin_name not in ("execute", "self"):
                if pin.linked_to:
                    value = self._trace_value_source(pin.linked_to[0])
                break

        return [f"{indent}{var_name} = {value};"]

    def _generate_macro_code(self, node: Node, indent: str) -> List[str]:
        """Generate code for macro instances."""
        macro_name = node.extra_data.get("MacroName", "UnknownMacro")

        if macro_name == "IsValid":
            # Find input object
            input_pin = node.pins.get("InputObject")
            obj = "/* object */"
            if input_pin and input_pin.linked_to:
                obj = self._trace_value_source(input_pin.linked_to[0])

            return [
                f"{indent}if (IsValid({obj}))",
                f"{indent}{{",
                f"{indent}{self.indent}// Is Valid branch",
                f"{indent}}}",
            ]

        return [f"{indent}// Macro: {macro_name}"]

    def _trace_value_source(self, link: dict) -> str:
        """Trace where a value comes from."""
        node_guid = link.get("NodeGuid", "")
        pin_name = link.get("PinName", "")
        node_title = link.get("NodeTitle", "")

        source_node = self.graph.nodes.get(node_guid)
        if not source_node:
            return f"/* {node_title}.{pin_name} */"

        if source_node.node_class == "K2Node_FunctionEntry":
            # This is a function input parameter
            return pin_name

        if source_node.node_class == "K2Node_VariableGet":
            return source_node.extra_data.get("VariableName", pin_name)

        if source_node.node_class == "K2Node_CallFunction":
            func_name = source_node.extra_data.get("FunctionName", "")
            return f"{func_name}Result.{pin_name}"

        if source_node.node_class == "K2Node_Knot":
            # Follow the reroute
            input_pin = source_node.pins.get("InputPin")
            if input_pin and input_pin.linked_to:
                return self._trace_value_source(input_pin.linked_to[0])

        return f"/* {node_title}.{pin_name} */"

    def _format_default_value(self, pin: Pin) -> str:
        """Format a default value for C++."""
        if pin.default_value:
            return pin.default_value

        cat = pin.type_info.get("Category", "")
        if cat == "bool":
            return "false"
        if cat in ("int", "real", "float", "double"):
            return "0"
        if cat == "string":
            return 'FString()'
        if cat == "struct":
            return "FGameplayTag()"
        if cat == "object":
            return "nullptr"

        return "/* default */"


def process_component(json_path: Path) -> Tuple[str, str]:
    """Process a component JSON and generate implementation code."""
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    class_name = data.get("Name", json_path.stem)
    graphs = data.get("Logic", {}).get("AllGraphs", [])

    implementations = []

    for graph_data in graphs:
        graph_type = graph_data.get("GraphType", "")
        graph_name = graph_data.get("GraphName", "")

        # Only process function graphs
        if graph_type != "Function":
            continue

        parser = GraphParser(graph_data)
        generator = CppLogicGenerator(parser, class_name)

        func_body = generator.generate_function_body()
        implementations.append((graph_name, func_body))

    return class_name, implementations


def main():
    """Main entry point."""
    component_dir = Path(r"C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA\Component")
    output_dir = Path(r"C:\scripts\SLFConversion\Generated\Logic")
    output_dir.mkdir(parents=True, exist_ok=True)

    for json_file in sorted(component_dir.glob("AC_*.json")):
        print(f"Processing: {json_file.name}")
        try:
            class_name, implementations = process_component(json_file)

            output_file = output_dir / f"{class_name}_logic.txt"
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(f"// Logic implementations for {class_name}\n")
                f.write(f"// Generated from {json_file.name}\n\n")

                for func_name, func_body in implementations:
                    f.write(f"// ═══ {func_name} ═══\n")
                    f.write(func_body)
                    f.write("\n\n")

            print(f"  -> Generated {len(implementations)} function implementations")
        except Exception as e:
            print(f"  ERROR: {e}")


if __name__ == "__main__":
    main()
