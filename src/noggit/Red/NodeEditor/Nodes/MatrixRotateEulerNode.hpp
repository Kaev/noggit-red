// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_MATRIXROTATEEULERNODE_HPP
#define NOGGIT_MATRIXROTATEEULERNODE_HPP

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
        class MatrixRotateEulerNode : public BaseNode
        {
        Q_OBJECT

        public:
            MatrixRotateEulerNode();
            NodeValidationState validate() override;
            void compute() override;
            QJsonObject save() const override;
            void restore(QJsonObject const& json_obj) override;

        private:

        };

    }

}

#endif //NOGGIT_MATRIXROTATEEULERNODE_HPP
