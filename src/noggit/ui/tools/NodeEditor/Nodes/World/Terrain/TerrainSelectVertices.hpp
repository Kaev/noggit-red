// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_TERRAINSELECTVERTICES_HPP
#define NOGGIT_TERRAINSELECTVERTICES_HPP

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
    class TerrainSelectVerticesNode : public ContextLogicNodeBase
    {
    Q_OBJECT

    public:
      TerrainSelectVerticesNode();
      void compute() override;
    };

  }

}

#endif //NOGGIT_TERRAINSELECTVERTICES_HPP
