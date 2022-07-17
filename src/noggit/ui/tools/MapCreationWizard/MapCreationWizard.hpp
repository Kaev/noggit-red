// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <noggit/ui/minimap_widget.hpp>
#include <noggit/ui/widget.hpp>
#include <noggit/World.h>
#include <bitset>
#include <noggit/ui/tools/MapCreationWizard/widgets/LocaleStringWidget.hpp>

namespace Noggit::Ui::Tools::MapCreationWizard
{
      enum class MapCreationFlags : uint8_t
      {
          EDIT_NONE = 0,
          EDIT_ADD = 1,
          EDIT_REMOVE = 2,
          EDIT_GROUP = 4,
          EDIT_SELECTION = 8,
      	  EDIT_MOVE = 16
      };

  class MapCreationWizard : public Noggit::Ui::widget
    {
      Q_OBJECT

    public:
      MapCreationWizard(std::shared_ptr<Project::NoggitProject> project, QWidget *parent = nullptr);
      ~MapCreationWizard();

      void wheelEvent(QWheelEvent *event) override;
      void destroyFakeWorld() { if(_world) delete _world; _world = nullptr; _minimap_widget->world (nullptr); };

    signals:
      void map_dbc_updated();

    private:
  	  std::shared_ptr<Project::NoggitProject> _project;
      Noggit::Ui::minimap_widget* _minimap_widget;
      int _selected_map;
      QGroupBox* _map_editor_buttons;
      QGroupBox* _map_settings;

      // Map settings
      QLineEdit* _directory;

      QCheckBox* _is_big_alpha;
      QCheckBox* _sort_by_size_cat;

      QComboBox* _instance_type;

      Widget::LocaleStringWidget* _map_name;

      QComboBox* _area_table_id;

      Widget::LocaleStringWidget* _map_desc_alliance;
      Widget::LocaleStringWidget* _map_desc_horde;

      QComboBox* _loading_screen;
      QDoubleSpinBox* _minimap_icon_scale;
      QComboBox *_corpse_map_id;

      QDoubleSpinBox* _corpse_x;
      QDoubleSpinBox* _corpse_y;

      QSpinBox* _time_of_day_override;
      QComboBox* _expansion_id;
      QSpinBox* _raid_offset;
      QSpinBox* _max_players;

      QPushButton* edit_remove_button;
      QPushButton* edit_add_button;
      QPushButton* edit_group_button;
      QPushButton* edit_selection_button;
      QPushButton* edit_move_button;

      FlagSet<MapCreationFlags> _mapCreationEditorFlags;

      World* _world = nullptr;

      bool _is_new_record = false;
      int _cur_map_id = -1;

      QMetaObject::Connection _connection;

      void selectMap(int map_id);

      void saveCurrentEntry();
      void discardChanges();

      void addNewMap();
      void removeMap();

    };
  }
