// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "TypeParameterNode.hpp"

#include <noggit/Red/NodeEditor/Nodes/BaseNode.inl>
#include <noggit/Red/NodeEditor/Nodes/DataTypes/GenericData.hpp>

using namespace noggit::Red::NodeEditor::Nodes;

TypeParameterNode::TypeParameterNode()
: BaseNode()
{
  setName("Data :: TypeParameter");
  setCaption("Data :: TypeParameter");
  _validation_state = NodeValidationState::Valid;

  addPort<AnyData>(PortType::In, "Any", true);
  addPort<StringData>(PortType::Out, "Type<String>", true);
}

void TypeParameterNode::compute()
{
  auto data = _in_ports[0].in_value.lock();

  _out_ports[0].out_value = std::make_shared<StringData>(data->type().parameter_type_id.toStdString());
  Q_EMIT dataUpdated(0);
}

NodeValidationState TypeParameterNode::validate()
{
  auto data = _in_ports[0].in_value.lock();

  if (!data)
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: failed to evaluate input.");
    return _validation_state;
  }

  return _validation_state;
}
