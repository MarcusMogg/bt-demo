#pragma once

#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h"
#include "behaviortree_cpp/bt_factory.h"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <memory>

namespace godot {
enum NodeStatus {
  IDLE = 0,
  RUNNING = 1,
  SUCCESS = 2,
  FAILURE = 3,
  SKIPPED = 4,
};
}

VARIANT_ENUM_CAST(NodeStatus);

namespace godot {

class BeehaveTree : public Node {
  GDCLASS(BeehaveTree, Node)

private:
  BT::BehaviorTreeFactory factory_;
  std::unique_ptr<BT::Tree> tree_;
  Node *actor_{nullptr};

protected:
  static void _bind_methods();

public:
  BeehaveTree();
  ~BeehaveTree();

  void _ready() override;
  void _process(double delta) override;

  void stop();
  void start(godot::String data);

private:
  int Tick();
  void RegisterTreeNode(Node *ch);
};

class BeehaveNode : public Node {
  GDCLASS(BeehaveNode, Node)

private:
  BT::TreeNode *node_{nullptr};
  godot::TypedArray<godot::String> input_ports_;
  godot::TypedArray<godot::String> output_ports_;

protected:
  static void _bind_methods();

public:
  BeehaveNode();
  ~BeehaveNode();

  void _ready() override;

  GDVIRTUAL1R(NodeStatus, _start, Node *);
  GDVIRTUAL1R(NodeStatus, _tick, Node *);
  GDVIRTUAL1(_halt, Node *);

  BT::NodeStatus Start(Node *actor, BT::TreeNode &node);
  BT::NodeStatus Tick(Node *actor);
  void Halt(Node *actor);

  void RegisterTreeNode(BT::TreeNode *node);

  BT::PortsList GetInputPorts() const;
  BT::PortsList GetOutputPorts() const;

  godot::TypedArray<godot::String> get_input_ports() { return input_ports_; }
  void set_input_ports(godot::TypedArray<godot::String> ports) {
    input_ports_ = ports;
  }

  godot::TypedArray<godot::String> get_output_ports() { return output_ports_; }
  void set_output_ports(godot::TypedArray<godot::String> ports) {
    output_ports_ = ports;
  }

  godot::Variant get_input(godot::String name);
  void set_output(godot::String name, godot::Variant data);
};
} // namespace godot
