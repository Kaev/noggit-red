// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/ZoneIDBrowser.h>

#include <noggit/DBC.h>
#include <noggit/Log.h>
#include <noggit/Misc.h>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>

#include <iostream>
#include <sstream>
#include <string>

namespace Noggit
{
  namespace Ui
  {
    zone_id_browser::zone_id_browser(std::shared_ptr<Noggit::Project::NoggitProject> project, QWidget* parent)
      : QWidget(parent), _project(project), _area_tree(new QTreeWidget()), mapID(-1)
    {
      auto layout = new QFormLayout(this);

      _radius_spin = new QDoubleSpinBox (this);
      _radius_spin->setRange (0.0f, 1000.0f);
      _radius_spin->setDecimals (2);
      _radius_spin->setValue (_radius);

      layout->addRow ("Radius:", _radius_spin);

      _radius_slider = new QSlider (Qt::Orientation::Horizontal, this);
      _radius_slider->setRange (0, 250);
      _radius_slider->setSliderPosition (_radius);

      layout->addRow (_radius_slider);
      layout->addRow (_area_tree);
      
      setMinimumWidth(250);

      connect ( _area_tree, &QTreeWidget::itemSelectionChanged
              , [this]
                {
                  auto const& selected_items = _area_tree->selectedItems();
                  if (selected_items.size())
                  {
                    emit selected (selected_items.back()->data(0, 1).toInt());
                  }
                }
              );

      connect ( _radius_spin, qOverload<double> (&QDoubleSpinBox::valueChanged)
          , [&] (double v)
                {
                  _radius = v;
                  QSignalBlocker const blocker(_radius_slider);
                  _radius_slider->setSliderPosition ((int)std::round (v));
                }
      );

      connect ( _radius_slider, &QSlider::valueChanged
          , [&] (int v)
                {
                  _radius = v;
                  QSignalBlocker const blocker(_radius_spin);
                  _radius_spin->setValue(v);
                }
      );
    }

    void zone_id_browser::setMapID(int id)
    {
      mapID = id;

      auto mapRecord = _project->ClientDatabase->MapRepository->GetMapById(id);

      std::stringstream ss;
      ss << id << "-" << mapRecord.Directory;
      _area_tree->setHeaderLabel(ss.str().c_str());

      buildAreaList();
    }

    void zone_id_browser::setZoneID(int id)
    {
      QSignalBlocker const block_area_tree(_area_tree);

      if (_items.find(id) != _items.end())
      {
        _area_tree->selectionModel()->clear();
        auto* item = _items.at(id);

        item->setSelected(true);

        while ((item = item->parent()))
        {
          item->setExpanded(true);
        }
      }
    }

    void zone_id_browser::buildAreaList()
    {
      QSignalBlocker const block_area_tree(_area_tree);
      _area_tree->clear();
      _area_tree->setColumnCount(1);
      _items.clear();

        auto areaList = _project->ClientDatabase->AreaTableRepository->GetAllAreasForMapId(mapID);
    	for(auto const& map : areaList)
        {
            add_area(map.Id);
        }
    }

    void zone_id_browser::changeRadius(float change)
    {
      _radius_spin->setValue (_radius + change);
    }

    void zone_id_browser::setRadius(float radius)
    {
      _radius_spin->setValue(radius);
    }

    QTreeWidgetItem* zone_id_browser::create_or_get_tree_widget_item(int area_id)
    {
      auto it = _items.find(area_id);

      if (it != _items.end())
      {
        return _items.at(area_id);
      }
      else
      {
        QTreeWidgetItem* item = new QTreeWidgetItem();

        std::stringstream ss;
        auto area_name = _project->ClientDatabase->AreaTableRepository->GetAreaName(area_id);
        ss << area_id << "-" << area_name[Locale::enUS];
        item->setData(0, 1, QVariant(area_id));
        item->setText(0, QString(ss.str().c_str()));
        _items.emplace(area_id, item);

        return item;
      }
    }

    QTreeWidgetItem* zone_id_browser::add_area(int area_id)
    {
      QTreeWidgetItem* item = create_or_get_tree_widget_item(area_id);

      std::uint32_t parent_area_id = _project->ClientDatabase->AreaTableRepository->GetParentAreaId(area_id);
      
      if (parent_area_id && parent_area_id != area_id)
      {
        QTreeWidgetItem* parent_item = add_area(parent_area_id);
        parent_item->addChild(item);
      }
      else
      {
        _area_tree->addTopLevelItem(item);
      }

      return item;
    }
  }
}
