// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_MOVESELECTEDOBJECTINSTANCES_HPP
#define NOGGIT_MOVESELECTEDOBJECTINSTANCES_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

namespace Noggit
{
  namespace Ui::Tools::NodeEditor::Nodes
  {
    class MoveSelectedObjectInstancesNode : public ContextLogicNodeBase
    {
    Q_OBJECT

    public:
      MoveSelectedObjectInstancesNode();
      void compute() override;
    };

  }

}

#endif //NOGGIT_MOVESELECTEDOBJECTINSTANCES_HPP
