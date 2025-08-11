#include "gdbt.h"
#include <cstddef>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void BeehaveTree::_bind_methods() {
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "actor"), "SetActor", "GetActor");
}

BeehaveTree::BeehaveTree() : actor_(nullptr) {
  // Initialize any variables here.
}

BeehaveTree::~BeehaveTree() {
  // Add your cleanup here.
}

void BeehaveTree::SetActor(Node *a) {
  actor_ = a;
  if (actor_ == nullptr) {
    actor_ = get_parent();
  }
}
Node *BeehaveTree::GetActor() const { return actor_; }

void BeehaveTree::_process(double delta) {}

void BeehaveNode::_bind_methods() {

  ClassDB::bind_method(D_METHOD("Tick", "actor"), &BeehaveNode::Tick);

  // ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "input_ports"), "SetInputPorts",
  //              "GetInputPorts");
}

BeehaveNode::BeehaveNode() {
  // Initialize any variables here.
}

BeehaveNode::~BeehaveNode() {
  // Add your cleanup here.
}
