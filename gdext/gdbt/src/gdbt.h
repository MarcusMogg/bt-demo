#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h"
#include "behaviortree_cpp/bt_factory.h"
#include <godot_cpp/classes/node.hpp>
#include <memory>
#include <vector>

namespace godot {

class BeehaveTree : public Node {
  GDCLASS(BeehaveTree, Node)

private:
  std::unique_ptr<BT::Tree> tree_;
  Node *actor_;

protected:
  static void _bind_methods();

public:
  BeehaveTree();
  ~BeehaveTree();

  void _process(double delta) override;

  void SetActor(Node *a);
  Node *GetActor() const;
};

class BeehaveNode : public Node {
  GDCLASS(BeehaveNode, Node)

private:
  std::unique_ptr<BT::SimpleActionNode> node_;
  std::vector<std::string> input_ports_;
  std::vector<std::string> output_ports_;

protected:
  static void _bind_methods();

public:
  BeehaveNode();
  ~BeehaveNode();

  virtual BT::NodeStatus Tick(Node *actor) { return BT::NodeStatus::SUCCESS; }
};

} // namespace godot

#endif