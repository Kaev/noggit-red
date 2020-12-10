// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_LOGICFORLOOPNODE_HPP
#define NOGGIT_LOGICFORLOOPNODE_HPP

#include "LogicNodeBase.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;


namespace noggit
{
    namespace Red::PresetEditor::Nodes
    {
        class LogicForLoopNode : public LogicNodeBase
        {
        Q_OBJECT

        public:
            LogicForLoopNode();
            void compute() override;
            NodeValidationState validate() override;
            QJsonObject save() const override;
            void restore(QJsonObject const& json_obj) override;

        private:
          QSpinBox* _n_iterations_default;

        }

    }

}

#endif //NOGGIT_LOGICFORLOOPNODE_HPP
