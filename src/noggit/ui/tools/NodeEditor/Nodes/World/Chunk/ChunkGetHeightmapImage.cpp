// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "ChunkGetHeightmapImage.hpp"

#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.inl>
#include <noggit/ui/tools/NodeEditor/Nodes/DataTypes/GenericData.hpp>
#include <cmath>
#include <limits>

using namespace Noggit::Ui::Tools::NodeEditor::Nodes;

ChunkGetHeightmapImageNode::ChunkGetHeightmapImageNode()
: ContextLogicNodeBase()
{
  setName("Chunk :: GetHeightmapImage");
  setCaption("Chunk :: GetHeightmapImage");
  _validation_state = NodeValidationState::Valid;

  addPortDefault<LogicData>(PortType::In, "Logic", true);
  addPortDefault<ChunkData>(PortType::In, "Chunk", true);
  addPortDefault<DecimalData>(PortType::In, "MinHeight<Decimal>", true);
  addPortDefault<DecimalData>(PortType::In, "MaxHeight<Decimal>", true);

  addPort<LogicData>(PortType::Out, "Logic", true);
  addPort<ImageData>(PortType::Out, "Image", true);
}

void ChunkGetHeightmapImageNode::compute()
{
  World* world = gCurrentContext->getWorld();
  gCurrentContext->getViewport()->makeCurrent();
  OpenGL::context::scoped_setter const _ (::gl, gCurrentContext->getViewport()->context());

  MapChunk* chunk = defaultPortData<ChunkData>(PortType::In, 1)->value();
  double min_height = defaultPortData<DecimalData>(PortType::In, 2)->value();
  double max_height = defaultPortData<DecimalData>(PortType::In, 3)->value();

  if (min_height > max_height || std::fabs(std::fabs(max_height) - std::fabs(min_height)) < std::numeric_limits<double>::epsilon())
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: incorrect precision bounds.");
    return;
  }

  _out_ports[0].out_value = std::make_shared<LogicData>(true);
  _node->onDataUpdated(0);

  _out_ports[1].out_value = std::make_shared<ImageData>(chunk->getHeightmapImage(static_cast<float>(min_height),
                                                                                 static_cast<float>(max_height)));
  _node->onDataUpdated(1);

}

NodeValidationState ChunkGetHeightmapImageNode::validate()
{

  if (!static_cast<ChunkData*>(_in_ports[1].in_value.lock().get()))
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: failed to evaluate chunk input.");
    return _validation_state;
  }

  return ContextLogicNodeBase::validate();
}