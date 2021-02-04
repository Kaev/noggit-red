// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_CROPWATERADTATPOS_HPP
#define NOGGIT_CROPWATERADTATPOS_HPP

#include <noggit/Red/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

namespace noggit
{
  namespace Red::NodeEditor::Nodes
  {
    class CropWaterADTAtPosNode : public ContextLogicNodeBase
    {
    Q_OBJECT

    public:
      CropWaterADTAtPosNode();
      void compute() override;
    };

  }

}

#endif //NOGGIT_CROPWATERADTATPOS_HPP
