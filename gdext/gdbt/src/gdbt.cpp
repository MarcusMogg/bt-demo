#include "gdbt.h"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h"
#include "behaviortree_cpp/bt_factory.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/print_string.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <cstddef>
#include <exception>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace std::string_view_literals;
namespace BT {
template <> godot::Variant convertFromString(StringView key) {

  return godot::Variant(key.data());
}
} // namespace BT

namespace {

class StatefulActionNodeWrapper : public BT::StatefulActionNode {
public:
  StatefulActionNodeWrapper(const std::string &name,
                            const BT::NodeConfig &config, godot::Node *actor,
                            godot::BeehaveNode *node)
      : StatefulActionNode(name, config), actor_(actor), node_(node) {}

  BT::NodeStatus onStart() override { return node_->Start(actor_, *this); }

  BT::NodeStatus onRunning() override { return node_->Tick(actor_); }

  void onHalted() override { node_->Halt(actor_); }

private:
  godot::Node *actor_;
  godot::BeehaveNode *node_;
};

} // namespace

void BeehaveTree::_bind_methods() {

  ClassDB::bind_method(D_METHOD("start", "data"), &BeehaveTree::start);
  ClassDB::bind_method(D_METHOD("stop"), &BeehaveTree::stop);
}

BeehaveTree::BeehaveTree() : actor_(nullptr) {
  // Initialize any variables here.
}

BeehaveTree::~BeehaveTree() {
  // Add your cleanup here.
}

void BeehaveTree::_ready() {
  // 编辑器下不要执行
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  if (actor_ == nullptr) {
    actor_ = get_parent();
  }
  // 获取所有BeehaveNode子节点, 先假设只有action，注册对应的action
  const auto &children = get_children();
  for (int i = 0; i < children.size(); ++i) {
    auto *ch = Object::cast_to<Node>(children.get(i));
    if (ch)
      RegisterTreeNode(ch);
  }

  godot::print_line("ready");
}

void BeehaveTree::_process(double) { Tick(); }

int BeehaveTree::Tick() {
  if (!actor_ || !tree_) {
    return -1;
  }
  try {
    tree_->tickOnce();
  } catch (const std::exception &e) {
    godot::print_error(e.what());
    tree_ = nullptr;
  }
  return 0;
}

void BeehaveTree::stop() {
  if (tree_) {
    tree_.reset();
  }
}
void BeehaveTree::start(godot::String data) {
  try {
    tree_ = std::make_unique<BT::Tree>(
        factory_.createTreeFromText(data.utf8().get_data()));
  } catch (const std::exception &e) {
    godot::print_error(e.what());
    tree_ = nullptr;
  }
}

void BeehaveTree::RegisterTreeNode(Node *ch) {
  if (auto *node = godot::Object::cast_to<godot::BeehaveNode>(ch);
      node != nullptr) {
    try {
      BT::PortsList ports;
      ports.merge(node->GetInputPorts());
      ports.merge(node->GetOutputPorts());

      factory_.registerNodeType<StatefulActionNodeWrapper>(
          node->get_name().validate_node_name().utf8().get_data(),
          std::move(ports), actor_, node);
    } catch (const std::exception &e) {
      godot::print_error(e.what());
    }
  }
}

void BeehaveNode::_bind_methods() {

  ClassDB::bind_method(D_METHOD("set_input_ports", "actor"),
                       &BeehaveNode::set_input_ports);
  ClassDB::bind_method(D_METHOD("get_input_ports"),
                       &BeehaveNode::get_input_ports);
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "input_ports"), "set_input_ports",
               "get_input_ports");

  ClassDB::bind_method(D_METHOD("set_output_ports", "actor"),
                       &BeehaveNode::set_output_ports);
  ClassDB::bind_method(D_METHOD("get_output_ports"),
                       &BeehaveNode::get_output_ports);
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "output_ports"), "set_output_ports",
               "get_output_ports");

  ClassDB::bind_method(D_METHOD("get_input", "name"), &BeehaveNode::get_input);
  ClassDB::bind_method(D_METHOD("set_output", "name", "data"),
                       &BeehaveNode::set_output);

  BIND_ENUM_CONSTANT(IDLE);
  BIND_ENUM_CONSTANT(RUNNING);
  BIND_ENUM_CONSTANT(SUCCESS);
  BIND_ENUM_CONSTANT(FAILURE);
  BIND_ENUM_CONSTANT(SKIPPED);
  GDVIRTUAL_BIND(_start, "actor");
  GDVIRTUAL_BIND(_tick, "actor");
  GDVIRTUAL_BIND(_halt, "actor");
}

BeehaveNode::BeehaveNode() {
  // Initialize any variables here.
}

BeehaveNode::~BeehaveNode() {
  // Add your cleanup here.
}
void BeehaveNode::_ready() {
  // 编辑器下不要执行
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
}

BT::PortsList BeehaveNode::GetInputPorts() const {
  BT::PortsList ports;
  for (int i = 0; i < input_ports_.size(); ++i) {
    auto name = static_cast<godot::String>(input_ports_.get(i));
    ports.insert(BT::InputPort<BT::AnyTypeAllowed>(name.utf8().get_data()));
  }
  return ports;
}
BT::PortsList BeehaveNode::GetOutputPorts() const {
  BT::PortsList ports;
  for (int i = 0; i < output_ports_.size(); ++i) {
    auto name = static_cast<godot::String>(output_ports_.get(i));
    ports.insert(BT::OutputPort<BT::AnyTypeAllowed>(name.utf8().get_data()));
  }
  TypedArray<Vector2> a;
  return ports;
}

BT::NodeStatus BeehaveNode::Start(Node *actor, BT::TreeNode &node) {
  RegisterTreeNode(&node);
  auto realret = BT::NodeStatus::FAILURE;
  if (NodeStatus ret{}; GDVIRTUAL_CALL(_start, actor, ret)) {
    realret = static_cast<BT::NodeStatus>(ret);
  }
  return realret;
}

BT::NodeStatus BeehaveNode::Tick(Node *actor) {
  auto realret = BT::NodeStatus::FAILURE;
  if (NodeStatus ret{}; GDVIRTUAL_CALL(_tick, actor, ret)) {
    realret = static_cast<BT::NodeStatus>(ret);
  }
  return realret;
}

void BeehaveNode::Halt(Node *actor) {
  GDVIRTUAL_CALL(_halt, actor);
  node_ = nullptr;
}

godot::Variant BeehaveNode::get_input(godot::String name) {
  if (node_) {
    auto ret = node_->getInput<Variant>(name.utf8().get_data());
    if (ret) {
      return ret.value();
    }
    godot::print_error("not find ", name, ret.error().c_str());
  } else {
    godot::print_error("node is nullptr");
  }
  return nullptr;
}
void BeehaveNode::set_output(godot::String name, godot::Variant data) {
  if (node_) {
    auto ret = node_->setOutput(name.utf8().get_data(), data);
    if (!ret) {
      godot::print_error(ret.error().c_str());
    }

  } else {
    godot::print_error("node is nullptr");
  }
}

void BeehaveNode::RegisterTreeNode(BT::TreeNode *node) { node_ = node; }