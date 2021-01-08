// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_VECTOR3DTOXYZNODE_HPP
#define NOGGIT_VECTOR3DTOXYZNODE_HPP

#include "BaseNode.hpp"

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
        class Vector3DToXYZNode : public BaseNode
        {
        Q_OBJECT

        public:
            Vector3DToXYZNode();
            NodeValidationState validate() override;
            void compute() override;
        };

    }

}

#endif //NOGGIT_VECTOR3DTOXYZNODE_HPP
