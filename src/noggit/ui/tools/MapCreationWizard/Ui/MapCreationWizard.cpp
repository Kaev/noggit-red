// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "MapCreationWizard.hpp"

#include <noggit/ui/FontAwesome.hpp>
#include <noggit/ui/windows/noggitWindow/NoggitWindow.hpp>
#include <noggit/project/CurrentProject.hpp>
#include <blizzard-database-library/include/structures/Types.h>
#include <noggit/MapView.h>
#include <noggit/World.h>
#include <noggit/application/Utils.hpp>
#include <noggit/Log.h>

#include <util/qt/overload.hpp>

#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QPushButton>
#include <QScrollArea>
#include <QWheelEvent>
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QStackedWidget>
#include <QDir>
#include <QMessageBox>

#include <filesystem>

using namespace Noggit::Ui::Tools::MapCreationWizard::Ui;

MapCreationWizard::MapCreationWizard(std::shared_ptr<Project::NoggitProject> project, QWidget* parent) : Noggit::Ui::widget(parent), _project(project)
{
  auto layout = new QHBoxLayout(this);

  // Left side
  auto layout_left = new QHBoxLayout(this);
  layout->addLayout(layout_left);

  auto scroll_minimap = new QScrollArea(this);

  _minimap_widget = new Noggit::Ui::minimap_widget(this);
  _minimap_widget->draw_boundaries(true);

  layout_left->addWidget(scroll_minimap);

  scroll_minimap->setAlignment(Qt::AlignCenter);
  scroll_minimap->setWidget(_minimap_widget);
  scroll_minimap->setWidgetResizable(true);

  // Right side
  auto layout_right_holder = new QWidget(this);
  layout_right_holder->setMinimumWidth(300);
  layout_right_holder->setMaximumWidth(550);
  auto layout_right = new QVBoxLayout (layout_right_holder);
  layout_right_holder->setLayout(layout_right);
  layout->addWidget(layout_right_holder);

  auto layout_selector_wgt = new QWidget(layout_right_holder);
  auto layout_selector = new QHBoxLayout(layout_selector_wgt);
  layout_selector_wgt->setLayout(layout_selector);

  layout_selector->setAlignment(Qt::AlignLeft);
  layout_right->addWidget(layout_selector_wgt);

  _corpse_map_id = new QComboBox(this);
  _corpse_map_id->addItem("None");
  _corpse_map_id->setItemData(0, QVariant (-1));

  // Fill selector combo
  auto maps = project->ClientDatabase->MapRepository->GetMapList();
  auto count = 1;
  for (auto& map : maps)
  {
      std::string name = map.Name[Locale::enUS];
      int map_id = map.Id;
      int area_type = map.InstanceType;

      if (area_type < 0 || area_type > 4 || !World::IsEditableWorld(map.Directory,map.Id))
          continue;

      _corpse_map_id->addItem(QString::number(map_id) + " - " + QString::fromUtf8(name.c_str()));
      _corpse_map_id->setItemData(count + 1, QVariant(map_id));

      count++;
  }

  auto add_btn = new QPushButton("New",this);
  add_btn->setIcon(Noggit::Ui::FontAwesomeIcon(Noggit::Ui::FontAwesome::plus));
  layout_selector->addWidget(add_btn);

  add_btn->setAccessibleName("map_wizard_add_button");

  auto remove_btn = new QPushButton("Remove",this);
  remove_btn->setIcon(Noggit::Ui::FontAwesomeIcon(Noggit::Ui::FontAwesome::times));
  layout_selector->addWidget(remove_btn);
  
  remove_btn->setAccessibleName("map_wizard_remove_button");

  _map_settings = new QGroupBox("Map settings", this);
  layout_right->addWidget(_map_settings);

  auto map_settings_layout = new QFormLayout(_map_settings);
  _map_settings->setLayout(map_settings_layout);

  _directory = new QLineEdit(_map_settings);
  map_settings_layout->addRow("Map directory:", _directory);

  _is_big_alpha = new QCheckBox(this);
  map_settings_layout->addRow("Big alpha:", _is_big_alpha);
  _is_big_alpha->setChecked(true);

  _sort_by_size_cat = new QCheckBox(this);
  map_settings_layout->addRow("Sort models", _sort_by_size_cat);
  _sort_by_size_cat->setChecked(true);
  _sort_by_size_cat->setToolTip("Sorts models based on their size on save. May help increase loading speed of the map.");


  _instance_type = new QComboBox(_map_settings);
  _instance_type->addItem("None");
  _instance_type->setItemData(0, QVariant(0));

  _instance_type->addItem("Instance");
  _instance_type->setItemData(1, QVariant(1));

  _instance_type->addItem("Raid");
  _instance_type->setItemData(2, QVariant(2));

  _instance_type->addItem("Battleground");
  _instance_type->setItemData(3, QVariant(3));

  _instance_type->addItem("Arena");
  _instance_type->setItemData(4, QVariant(4));

  map_settings_layout->addRow("Map type:",_instance_type);

  _map_name = new LocaleDBCEntry(_map_settings);
  map_settings_layout->addRow("Map name:",_map_name);

  _area_table_id = new QSpinBox(_map_settings);
  map_settings_layout->addRow("Area ID:",_area_table_id);
  _area_table_id->setMaximum(std::numeric_limits<std::int32_t>::max());

  _map_desc_alliance = new LocaleDBCEntry(_map_settings);
  map_settings_layout->addRow("Description (Alliance):",_map_desc_alliance);

  _map_desc_horde = new LocaleDBCEntry(_map_settings);
  map_settings_layout->addRow("Description (Horde):",_map_desc_horde);

  _loading_screen  = new QSpinBox(_map_settings);
  map_settings_layout->addRow("Loading screen:",_loading_screen);
  _loading_screen->setMaximum(std::numeric_limits<std::int32_t>::max());

   _minimap_icon_scale = new QDoubleSpinBox(_map_settings);
  map_settings_layout->addRow("Minimap icon scale:",_minimap_icon_scale);

  _corpse_map_id->setCurrentText("None");
  map_settings_layout->addRow("Corpse map:",_corpse_map_id);

  _corpse_x = new QDoubleSpinBox(_map_settings);
  map_settings_layout->addRow("Corpse X:",_corpse_x);
  _corpse_x->setMinimum(-17066.66656); // map size
  _corpse_x->setMaximum(17066.66656);

  _corpse_y = new QDoubleSpinBox(_map_settings);
  map_settings_layout->addRow("Corpse Y:",_corpse_y);
  _corpse_y->setMinimum(-17066.66656); // map size
  _corpse_y->setMaximum(17066.66656);

  _time_of_day_override = new QSpinBox(_map_settings);
  _time_of_day_override->setMinimum(-1);
  _time_of_day_override->setMaximum(2880); // Time Values from 0 to 2880 where each number represents a half minute from midnight to midnight 
  _time_of_day_override->setValue(-1);

  map_settings_layout->addRow("Daytime override:",_time_of_day_override);

  _expansion_id = new QComboBox(_map_settings);

  _expansion_id->addItem("Classic");
  _expansion_id->setItemData(0, QVariant(0));

  _expansion_id->addItem("Burning Crusade");
  _expansion_id->setItemData(1, QVariant(1));

  _expansion_id->addItem("Wrath of the Lich King");
  _expansion_id->setItemData(2, QVariant(2));

  map_settings_layout->addRow("Expansion:",_expansion_id);

  _raid_offset = new QSpinBox(_map_settings);
  _raid_offset->setMaximum(std::numeric_limits<std::int32_t>::max());
  map_settings_layout->addRow("Raid offset:",_raid_offset);

  _max_players = new QSpinBox(_map_settings);
  _max_players->setMaximum(std::numeric_limits<std::int32_t>::max());
  map_settings_layout->addRow("Max players:",_max_players);

  // Bottom row
  auto bottom_row_wgt = new QWidget(layout_right_holder);
  auto btn_row_layout = new QHBoxLayout(layout_right_holder);
  bottom_row_wgt->setLayout(btn_row_layout);
  btn_row_layout->setAlignment(Qt::AlignRight);

  auto save_btn = new QPushButton("Save", this);
  auto discard_btn = new QPushButton("Discard", this);
  btn_row_layout->addWidget(save_btn);
  btn_row_layout->addWidget(discard_btn);
  save_btn->setAccessibleName("map_wizard_save_button");
  discard_btn->setAccessibleName("map_wizard_discard_button");

  layout_right->addWidget(bottom_row_wgt);

  // Connections

  connect(save_btn, &QPushButton::clicked
      ,[&] ()
      {
        saveCurrentEntry();
      });

  connect(discard_btn, &QPushButton::clicked
      ,[&] ()
      {
        discardChanges();
      });

  connect(add_btn, &QPushButton::clicked
      ,[&] ()
          {
            addNewMap();
          });

  connect(remove_btn, &QPushButton::clicked
      ,[&] ()
          {
            removeMap();
          });

  _connection = connect(reinterpret_cast<Noggit::Ui::Windows::NoggitWindow*>(parent),
                        QOverload<int>::of(&Noggit::Ui::Windows::NoggitWindow::mapSelected)
                        , [&] (int index)
                              {
                                selectMap(index);
                              }
  );

  // Selection

  QObject::connect
      ( _minimap_widget,  &Noggit::Ui::minimap_widget::tile_clicked
          , [this] (QPoint tile)
        {
          if (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier))
          {
            int x = tile.x() - 1;
            int y = tile.y() - 1;

            for (int i = 0; i < 3; ++i)
            {
              for (int j = 0; j < 3; ++j)
              {
                int x_final = x + i;
                int y_final = y + j;

                if (x_final < 0 || x_final > 63 || y_final < 0 || y_final > 63)
                  continue;

                if (!_world->mapIndex.hasTile(TileIndex(x_final, y_final)))
                {
                  if (!QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
                  {
                    _world->mapIndex.addTile(TileIndex(x_final, y_final));
                  }
                }
                else if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
                {
                  _world->mapIndex.removeTile(TileIndex(x_final, y_final));
                }
              }
            }
          }
          else
          {
            int x = tile.x();
            int y = tile.y();

            if (x < 0 || x > 63 || y < 0 || y > 63)
              return;

            if (!_world->mapIndex.hasTile(TileIndex(x, y)))
            {
              if (!QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
              {
                _world->mapIndex.addTile(TileIndex(x, y));
              }
            }
            else if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
            {
              _world->mapIndex.removeTile(TileIndex(x, y));
            }
          }

          update();
        }
      );

}

void MapCreationWizard::selectMap(int map_id)
{
  _is_new_record = false;
  _cur_map_id = map_id;

  if (_world)
  {
    delete _world;
  }

  auto mapEntry = _project->ClientDatabase->MapRepository->GetMapById(map_id);

  _world = new World(_project,mapEntry.Directory, map_id, Noggit::NoggitRenderContext::MAP_VIEW);
  _minimap_widget->world(_world);
  _directory->setText(QString::fromStdString(mapEntry.Directory));
  _directory->setEnabled(false);
  _is_big_alpha->setChecked(_world->mapIndex.hasBigAlpha());
  _is_big_alpha->setEnabled(false);
  _sort_by_size_cat->setChecked(_world->mapIndex.sort_models_by_size_class());
  _instance_type->setCurrentIndex(mapEntry.InstanceType);
  _map_name->fill(mapEntry.Name);
  _map_desc_alliance->fill(mapEntry.AllianceMapDescription);
  _map_desc_horde->fill(mapEntry.HordeMapDescription);
  _area_table_id->setValue(mapEntry.AreaTableId);
  _loading_screen->setValue(mapEntry.LoadingScreenId);
  _minimap_icon_scale->setValue(mapEntry.MiniMapScale);

  int corpse_map_idx = mapEntry.CorpseMapId;
  for (int i = 0; i < _corpse_map_id->count(); ++i)
  {
    if (_corpse_map_id->itemData(i) == corpse_map_idx)
    {
      _corpse_map_id->setCurrentIndex(i);
    }
  }

  _corpse_x->setValue(mapEntry.Corpse_X);
  _corpse_y->setValue(mapEntry.Corpse_Y);
  _time_of_day_override->setValue(mapEntry.TimeDayOverride);
  _expansion_id->setCurrentIndex(mapEntry.ExpansionId);

  //_raid_offset->setValue(record.getInt(64)); only ever used in 2 places? not sure what for

  _max_players->setValue(mapEntry.MaxPlayers);
}

void MapCreationWizard::wheelEvent(QWheelEvent* event)
{

  if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
  {
    const int degrees = event->angleDelta().y() / 8;
    int steps = degrees / 15;

    auto base_size = _minimap_widget->width();

    if (steps > 0)
    {
      auto new_size = std::max(512, base_size + 64);
      _minimap_widget->setFixedSize(new_size, new_size);
    } else
    {
      auto new_size = std::min(4096, base_size - 64);
      _minimap_widget->setFixedSize(new_size, new_size);
    }

    event->ignore();
  }

}

void MapCreationWizard::saveCurrentEntry()
{

  if (_is_new_record)
  {
    auto map_internal_name = _directory->text().toStdString();

    // Check if map with that name already exists
    bool mapFound = _project->ClientDatabase->MapRepository->DoesMapAlreadyExistWithName(map_internal_name);

    if (mapFound)
    {
      QMessageBox prompt;
      prompt.setIcon (QMessageBox::Warning);
      prompt.setWindowFlags(Qt::WindowStaysOnTopHint);
      prompt.setText ("Warning!");
      prompt.setInformativeText ("Saving failed. The map with this directory name already exists. Rename it to save.");
      prompt.addButton ("Okay", QMessageBox::AcceptRole);
      prompt.setWindowFlags (Qt::CustomizeWindowHint | Qt::WindowTitleHint);

      prompt.exec();
      return;
    }

    // Create WDT empty file for new map
    std::stringstream filename;
    filename << "World\\Maps\\" << map_internal_name << "\\" << map_internal_name << ".wdt";

    auto project_path = std::filesystem::path(Noggit::Project::CurrentProject::get()->ProjectPath.c_str());

    QDir dir((project_path / "/world/maps/" / map_internal_name).string().c_str());

    if (!dir.exists())
      dir.mkpath(".");

    auto filepath = project_path / BlizzardArchive::ClientData::normalizeFilenameInternal (filename.str());

    QFile file(filepath.string().c_str());
    file.open(QIODevice::WriteOnly);
    file.close();
  }

  // Save ADTs and WDT to disk
  _world->mapIndex.setBigAlpha(_is_big_alpha->isChecked());
  _world->setBasename(_directory->text().toStdString());
  _world->mapIndex.set_sort_models_by_size_class(_sort_by_size_cat->isChecked());
  _world->mapIndex.saveChanged(_world, true);
  _world->mapIndex.save();

  if (_is_new_record)
  {
      auto newMap = Noggit::Database::Repositories::MapEntry();
      newMap.Directory = _directory->text().toStdString();
      newMap.InstanceType = _instance_type->itemData(_instance_type->currentIndex()).toInt();
      newMap.AreaTableId = _area_table_id->value();
      newMap.LoadingScreenId = _loading_screen->value();
      newMap.CorpseMapId = _corpse_map_id->itemData(_corpse_map_id->currentIndex()).toInt();
      newMap.TimeDayOverride = _time_of_day_override->value();
      newMap.ExpansionId = _expansion_id->itemData(_expansion_id->currentIndex()).toInt();
      newMap.RaidOffset = _raid_offset->value();
      newMap.MaxPlayers = _max_players->value();
      newMap.MiniMapScale = static_cast<float>(_minimap_icon_scale->value());
      newMap.Corpse_X = static_cast<float>(_corpse_x->value());
      newMap.Corpse_Y = static_cast<float>(_corpse_y->value());

      newMap.Name = LocaleString();
      newMap.AllianceMapDescription = LocaleString();
      newMap.HordeMapDescription = LocaleString();

      _map_name->toRecord(newMap.Name);
      _map_desc_alliance->toRecord(newMap.AllianceMapDescription);
      _map_desc_horde->toRecord(newMap.HordeMapDescription);

      _project->ClientDatabase->MapRepository->CreateMap(newMap);
  }
  else
  {
      auto currentMap = _project->ClientDatabase->MapRepository->GetMapById(_cur_map_id);

      currentMap.Directory = _directory->text().toStdString();
      currentMap.InstanceType = _instance_type->itemData(_instance_type->currentIndex()).toInt();
      currentMap.AreaTableId = _area_table_id->value();
      currentMap.LoadingScreenId = _loading_screen->value();
      currentMap.CorpseMapId = _corpse_map_id->itemData(_corpse_map_id->currentIndex()).toInt();
      currentMap.TimeDayOverride = _time_of_day_override->value();
      currentMap.ExpansionId = _expansion_id->itemData(_expansion_id->currentIndex()).toInt();
      currentMap.RaidOffset = _raid_offset->value();
      currentMap.MaxPlayers = _max_players->value();
      currentMap.MiniMapScale = static_cast<float>(_minimap_icon_scale->value());
      currentMap.Corpse_X = static_cast<float>(_corpse_x->value());
      currentMap.Corpse_Y = static_cast<float>(_corpse_y->value());

      _map_name->toRecord(currentMap.Name);
      _map_desc_alliance->toRecord(currentMap.AllianceMapDescription);
      _map_desc_horde->toRecord(currentMap.HordeMapDescription);

      _project->ClientDatabase->MapRepository->SaveMap(currentMap);
  }

  emit map_dbc_updated();

  _is_new_record = false;

}

void MapCreationWizard::discardChanges()
{
  if (!_is_new_record)
  {
    selectMap(_cur_map_id);
  }
  else
  {
    addNewMap();
  }

}

MapCreationWizard::~MapCreationWizard()
{
  delete _world;
  disconnect(_connection);
}

void MapCreationWizard::addNewMap()
{
  _is_new_record = true;

  auto newMap = Noggit::Database::Repositories::MapEntry();
  newMap.Directory = "New_Map";
  newMap.InstanceType = 1;
  newMap.AreaTableId = 0;
  newMap.LoadingScreenId = 0;
  newMap.CorpseMapId = 0;
  newMap.TimeDayOverride = 0;
  newMap.ExpansionId = 0;
  newMap.RaidOffset = 0;
  newMap.MaxPlayers = 0;
  newMap.MiniMapScale = 1.0f;
  newMap.Corpse_X = 0.0f;
  newMap.Corpse_Y = 0.0f;

  newMap.Name = LocaleString();
  newMap.AllianceMapDescription = LocaleString();
  newMap.HordeMapDescription = LocaleString();

  auto createdMap =_project->ClientDatabase->MapRepository->CreateMap(newMap);
  _cur_map_id = createdMap.Id;

  if (_world)
  {
    delete _world;
  }

  _world = new World(_project,"New_Map", _cur_map_id, Noggit::NoggitRenderContext::MAP_VIEW, true);
  _minimap_widget->world(_world);

  _directory->setText("New_Map");
  _directory->setEnabled(true);

  _is_big_alpha->setChecked(true);
  _is_big_alpha->setEnabled(true);

  _sort_by_size_cat->setChecked(true);

  _instance_type->setCurrentIndex(0);

  _map_name->clear();
  _area_table_id->setValue(0);

  _map_desc_alliance->clear();
  _map_desc_horde->clear();

  _loading_screen->setValue(0);
  _minimap_icon_scale->setValue(0.0f);

  _corpse_map_id->setCurrentIndex(0);


  _corpse_x->setValue(0.0f);
  _corpse_y->setValue(0.0f);
  _time_of_day_override->setValue(0);
  _expansion_id->setCurrentIndex(0);
  _raid_offset->setValue(0);
  _max_players->setValue(0);
}

void MapCreationWizard::removeMap()
{
  if (!_is_new_record && _cur_map_id >= 0)
  {
      _project->ClientDatabase->MapRepository->DeleteMap(_cur_map_id);
  }

  emit map_dbc_updated();

  addNewMap();
}


LocaleDBCEntry::LocaleDBCEntry(QWidget* parent) : QWidget(parent)
{
  auto layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  _show_entry = new QStackedWidget(this);

  _en = new QLineEdit(this);
  _kr = new QLineEdit(this);
  _fr = new QLineEdit(this);
  _de = new QLineEdit(this);
  _cn = new QLineEdit(this);
  _tw = new QLineEdit(this);
  _es = new QLineEdit(this);
  _mx = new QLineEdit(this);
  _ru = new QLineEdit(this);
  _jp = new QLineEdit(this);
  _pt = new QLineEdit(this);
  _it = new QLineEdit(this);

  _unk1 = new QLineEdit(this);
  _unk2 = new QLineEdit(this);
  _unk3 = new QLineEdit(this);
  _unk4 = new QLineEdit(this);

  _flags = new QSpinBox(this);
  _flags->setVisible(false);

  _show_entry->addWidget(_en);
  _show_entry->addWidget(_kr);
  _show_entry->addWidget(_fr);
  _show_entry->addWidget(_de);
  _show_entry->addWidget(_cn);
  _show_entry->addWidget(_tw);
  _show_entry->addWidget(_es);
  _show_entry->addWidget(_mx);
  _show_entry->addWidget(_ru);
  _show_entry->addWidget(_jp);
  _show_entry->addWidget(_pt);
  _show_entry->addWidget(_it);
  _show_entry->addWidget(_unk1);
  _show_entry->addWidget(_unk2);
  _show_entry->addWidget(_unk3);
  _show_entry->addWidget(_unk4);

  layout->addWidget(_show_entry);

  _current_locale = new QComboBox(this);

  for (auto const &loc : _locale_names)
  {
    _current_locale->addItem(QString::fromStdString(loc));
  }

  _widget_map = {

      {"enUS", _en},
      {"koKR", _kr},
      {"frFR", _fr},
      {"deDE", _de},
      {"zhCN", _cn},
      {"zhTW", _tw},
      {"esES", _es},
      {"esMX", _mx},
      {"ruRU", _ru},
      {"jaJP", _jp},
      {"ptPT", _pt},
      {"itIT", _it},
      {"Unknown 1", _unk1},
      {"Unknown 2", _unk2},
      {"Unknown 3", _unk3},
      {"Unknown 4", _unk4}
  };

  layout->addWidget(_current_locale);
  // Connect

  connect ( _current_locale, &QComboBox::currentTextChanged
      , [&] (QString s)
            {
              setCurrentLocale(_current_locale->currentText().toStdString());
            }
  );

  setMaximumHeight(_en->height());
}

void LocaleDBCEntry::setCurrentLocale(const std::string& locale)
{
  _show_entry->setCurrentWidget(_widget_map.at(locale));
}

void LocaleDBCEntry::fill(DBCFile::Record& record, size_t field)
{
  for (int loc = 0; loc < 16; ++loc)
  {
    setValue(record.getLocalizedString(field, loc), loc);
  }

  _flags->setValue(record.getInt(field + 16));
}

void LocaleDBCEntry::fill(Noggit::LocaleString& record)
{
   for (int loc = 0; loc < 16; ++loc)
   {
       auto locale = LocaleNames[loc];
       setValue(record[locale], loc);
   }

   _flags->setValue(record.flags);
}

void LocaleDBCEntry::toRecord(Noggit::LocaleString& record)
{
  for (int loc = 0; loc < 16; ++loc)
  {
      auto locale = LocaleNames[loc];
      record.SetString(locale,getValue(loc));
  }
  record.flags = _flags->value();
}

void LocaleDBCEntry::clear()
{
  for (int loc = 0; loc < 16; ++loc)
  {
    setValue("", loc);
  }

  _flags->setValue(0);
}



