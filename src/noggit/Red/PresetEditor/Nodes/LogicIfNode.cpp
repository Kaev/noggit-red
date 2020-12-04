#include "LogicIfNode.hpp"

#include "BaseNode.inl"
#include "Data/GenericData.hpp"

using namespace noggit::Red::PresetEditor::Nodes;

LogicIfNode::LogicIfNode()
: BaseNode()
{
  setName("LogicIfNode");
  setCaption("If / Else");
  _validation_state = NodeValidationState::Valid;

  addPort<LogicData>(PortType::In, "Logic", true);
  addPort<BooleanData>(PortType::In, "Boolean", true);
  addPort<LogicData>(PortType::Out, "Then", true, ConnectionPolicy::One);
  addPort<LogicData>(PortType::Out, "Else", true, ConnectionPolicy::One);

  setIsLogicNode(true);
  setNLogicBranches(2);

}

void LogicIfNode::compute()
{
  auto logic = static_cast<BooleanData*>(_in_ports[0].in_value.lock().get());

  if (!logic)
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: Failed to evaluate logic input");

    _out_ports[0].out_value = std::make_shared<LogicData>(false);
    _out_ports[1].out_value = std::make_shared<LogicData>(false);
    Q_EMIT dataUpdated(0);
    Q_EMIT dataUpdated(1);

    setLogicBranchToExecute(-1);

    return;
  }

  setValidationState(NodeValidationState::Valid);

  if(!logic->value())
  {
    setLogicBranchToExecute(-1);
    return;
  }

  auto in_bool = _in_ports[1].in_value.lock();
  auto in_bool_ptr = static_cast<BooleanData*>(in_bool.get());

  if (!in_bool_ptr)
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Missing boolean input.");
    setLogicBranchToExecute(-1);
    return;
  }

  if (in_bool_ptr->value())
  {
    _out_ports[0].out_value = std::make_shared<LogicData>(true);
    _out_ports[1].out_value = std::make_shared<LogicData>(false);
    setLogicBranchToExecute(0);
  }
  else
  {
    _out_ports[0].out_value = std::make_shared<LogicData>(false);
    _out_ports[1].out_value = std::make_shared<LogicData>(true);
    setLogicBranchToExecute(1);
  }

  Q_EMIT dataUpdated(0);
  Q_EMIT dataUpdated(1);

  setValidationState(NodeValidationState::Warning);
  setValidationMessage(in_bool_ptr->value() ? "Debug: true" : "Debug: false");
}


