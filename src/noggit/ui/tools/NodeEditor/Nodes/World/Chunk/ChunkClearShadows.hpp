// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_CHUNKCLEARSHADOWS_HPP
#define NOGGIT_CHUNKCLEARSHADOWS_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

namespace noggit
{
  namespace ui::tools::NodeEditor::Nodes
  {
    class ChunkClearShadowsNode : public ContextLogicNodeBase
    {
    Q_OBJECT

    public:
      ChunkClearShadowsNode();
      void compute() override;
      NodeValidationState validate() override;
    };

  }

}

#endif //NOGGIT_CHUNKCLEARSHADOWS_HPP