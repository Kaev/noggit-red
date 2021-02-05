// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "LogicForLoopNode.hpp"

#include <noggit/Red/NodeEditor/Nodes/BaseNode.inl>
#include <noggit/Red/NodeEditor/Nodes/DataTypes/GenericData.hpp>

using namespace noggit::Red::NodeEditor::Nodes;

LogicForLoopNode::LogicForLoopNode()
: LogicNodeBase()
{
  setName("Logic :: ForLoop");
  setCaption("Logic:: ForLoop");
  _validation_state = NodeValidationState::Valid;
  setInterpreterToken(NodeInterpreterTokens::FOR);

  addPortDefault<LogicData>(PortType::In, "Logic", true);
  addPortDefault<UnsignedIntegerData>(PortType::In, "Times", true);

  addPort<LogicData>(PortType::Out, "Logic", true, ConnectionPolicy::One);
  addPort<UnsignedIntegerData>(PortType::Out, "Index", true, ConnectionPolicy::Many);

  setIsIterable(true);
}

void LogicForLoopNode::compute()
{
  auto logic = static_cast<LogicData*>(_in_ports[0].in_value.lock().get());

  if(!logic->value())
    return;

  auto n_iterations_ptr = static_cast<IntegerData*>(_in_ports[1].in_value.lock().get());

  unsigned n_iterations = n_iterations_ptr ? n_iterations_ptr->value() : _n_iterations_default->value();

  if (!n_iterations)
  {
    setIterationIndex(-1);
    return;
  }

  if (_iteration_index == n_iterations)
  {
    setIterationIndex(-1);
    return;
  }

  _out_ports[0].out_value = std::make_shared<LogicData>(true);
  _node->onDataUpdated(0);

  _out_ports[1].out_value = std::make_shared<UnsignedIntegerData>(getIterationindex());
  _node->onDataUpdated(1);

  setIterationIndex(getIterationindex() + 1);

}

QJsonObject LogicForLoopNode::save() const
{
  QJsonObject json_obj;

  json_obj["name"] = name();
  json_obj["caption"] = caption();
  json_obj["n_iterations"] = _n_iterations_default->value();

  return json_obj;
}

void LogicForLoopNode::restore(const QJsonObject& json_obj)
{
  setName(json_obj["name"].toString());
  setCaption(json_obj["caption"].toString());
  _n_iterations_default->setValue(json_obj["n_iterations"].toInt());
}

NodeValidationState LogicForLoopNode::validate()
{
  setValidationState(NodeValidationState::Valid);

  auto logic = static_cast<LogicData*>(_in_ports[0].in_value.lock().get());

  if (!logic)
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: Failed to evaluate logic input.");

    _out_ports[0].out_value = std::make_shared<LogicData>(false);
    _node->onDataUpdated(0);
  }

  auto n_iterations_ptr = static_cast<IntegerData*>(_in_ports[1].in_value.lock().get());

  int n_iterations = n_iterations_ptr ? n_iterations_ptr->value() : _n_iterations_default->value();

  if (!n_iterations)
  {
    setIterationIndex(-1);
  }

  setIterationIndex(0);
  setNIterations(std::max(0, n_iterations));

  return _validation_state;
}
