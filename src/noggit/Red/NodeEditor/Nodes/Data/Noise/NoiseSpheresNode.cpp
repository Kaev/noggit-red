// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "NoiseSpheresNode.hpp"

#include <noggit/Red/NodeEditor/Nodes/BaseNode.inl>
#include <noggit/Red/NodeEditor/Nodes/DataTypes/GenericData.hpp>

using namespace noggit::Red::NodeEditor::Nodes;

NoiseSpheresNode::NoiseSpheresNode()
: NoiseGeneratorBase()
{
  setName("Noise :: Spheres");
  setCaption("Noise :: Spheres");
  _validation_state = NodeValidationState::Valid;

  addPortDefault<DecimalData>(PortType::In, "Frequency<Decimal>", true);
  auto frequency = static_cast<QDoubleSpinBox*>(_in_ports[0].default_widget);
  frequency->setMinimum(0.0);
  frequency->setValue(1.0);

  addPort<NoiseData>(PortType::Out, "Noise", true);
}

void NoiseSpheresNode::compute()
{
  auto module = new noise::module::Spheres();

  double frequency = defaultPortData<DecimalData>(PortType::In, 0)->value();
  if (!checkBounds(frequency, 0.0, std::numeric_limits<double>::max(), "Frequency"))
    return;
  module->SetFrequency(frequency);

  std::shared_ptr<noise::module::Module> noise_data;
  noise_data.reset(module);
  _out_ports[0].out_value = std::make_shared<NoiseData>(noise_data);

  Q_EMIT dataUpdated(0);
}
