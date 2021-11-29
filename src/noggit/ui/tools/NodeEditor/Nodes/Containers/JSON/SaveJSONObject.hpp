// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_SAVEJSONOBJECT_HPP
#define NOGGIT_SAVEJSONOBJECT_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/LogicNodeBase.hpp>

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
        class SaveJSONObjectNode : public LogicNodeBase
        {
        Q_OBJECT

        public:
            SaveJSONObjectNode();
            void compute() override;
            NodeValidationState validate() override;
            QJsonObject save() const override;
            void restore(QJsonObject const& json_obj) override;
        };

    }

}

#endif //NOGGIT_SAVEJSONOBJECT_HPP
