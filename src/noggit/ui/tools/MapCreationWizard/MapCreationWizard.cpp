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
#include <QToolBar>
#include <QToolButton>
#include <QSize>
#include <QSizePolicy>
#include <QTableWidget>

#include "noggit/ui/FontNoggit.hpp"

namespace Noggit::Ui::Tools::MapCreationWizard
{
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

        //Right Side Buttons
        auto layout_right_holder_buttons = new QWidget(this);
        layout_right_holder_buttons->setMinimumWidth(40);
        layout_right_holder_buttons->setMaximumWidth(40);
        layout_right_holder_buttons->setMinimumHeight(30 * 5 + (6 * 5));
        layout_right_holder_buttons->setMaximumHeight(30 * 5 + (6 * 5));
        layout_right_holder_buttons->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
        auto layout_right_button = new QVBoxLayout(layout_right_holder_buttons);
        layout_right_button->setMargin(5);
        layout_right_button->setAlignment(Qt::AlignTop);

        edit_selection_button = new QPushButton(layout_right_holder_buttons);
        edit_selection_button->setMinimumSize(30, 30);
        edit_selection_button->setMaximumSize(30, 30);
        edit_selection_button->setCheckable(true);
        edit_selection_button->setChecked(true);
        edit_selection_button->setToolTip("selection");
        edit_selection_button->setIcon(Noggit::Ui::FontAwesomeIcon(Noggit::Ui::FontAwesome::Icons::mousepointer));
        edit_selection_button->setIconSize(QSize(20, 20));

        layout_right_button->addWidget(edit_selection_button);

        edit_add_button = new QPushButton(layout_right_holder_buttons);
        edit_add_button->setMinimumSize(30, 30);
        edit_add_button->setMaximumSize(30, 30);
        edit_add_button->setCheckable(true);
        edit_add_button->setChecked(false);
        edit_add_button->setToolTip("Add");
        edit_add_button->setIcon(Noggit::Ui::FontAwesomeIcon(Noggit::Ui::FontAwesome::Icons::plus));
        edit_add_button->setIconSize(QSize(20, 20));

        layout_right_button->addWidget(edit_add_button);

        edit_remove_button = new QPushButton(layout_right_holder_buttons);
        edit_remove_button->setMinimumSize(30, 30);
        edit_remove_button->setMaximumSize(30, 30);
        edit_remove_button->setCheckable(true);
        edit_remove_button->setChecked(false);
        edit_remove_button->setToolTip("Remove");
        edit_remove_button->setIcon(Noggit::Ui::FontNoggitIcon(Noggit::Ui::FontNoggit::Icons::minus));
        edit_remove_button->setIconSize(QSize(20, 20));

        layout_right_button->addWidget(edit_remove_button);


        edit_move_button = new QPushButton(layout_right_holder_buttons);
        edit_move_button->setMinimumSize(30, 30);
        edit_move_button->setMaximumSize(30, 30);
        edit_move_button->setCheckable(true);
        edit_move_button->setChecked(false);
        edit_move_button->setToolTip("Move");
        edit_move_button->setIcon(Noggit::Ui::FontNoggitIcon(Noggit::Ui::FontNoggit::Icons::GIZMO_TRANSLATE));
        edit_move_button->setIconSize(QSize(24, 24));

        layout_right_button->addWidget(edit_move_button);

        edit_group_button = new QPushButton(layout_right_holder_buttons);
        edit_group_button->setMinimumSize(30, 30);
        edit_group_button->setMaximumSize(30, 30);
        edit_group_button->setCheckable(true);
        edit_group_button->setChecked(false);
        edit_group_button->setToolTip("group");
        edit_group_button->setIcon(Noggit::Ui::FontNoggitIcon(Noggit::Ui::FontNoggit::Icons::GIZMO_SCALE));
        edit_group_button->setIconSize(QSize(20, 20));

        layout_right_button->addWidget(edit_group_button);


        layout_right_holder_buttons->setLayout(layout_right_button);
        layout->addWidget(layout_right_holder_buttons);

        _mapCreationEditorFlags = FlagSet<MapCreationFlags>();
        _mapCreationEditorFlags.set(MapCreationFlags::EDIT_SELECTION);

        connect(edit_move_button, &QPushButton::clicked, [&]
            {
                if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_MOVE))
                {
                    _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_MOVE);
                }
                else
                {
                    _mapCreationEditorFlags.set(MapCreationFlags::EDIT_MOVE);
                    if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_ADD)
                        || _mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_REMOVE)
                        || _mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_SELECTION))
                    {
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_REMOVE);
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_ADD);
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_SELECTION);
                        edit_remove_button->setChecked(false);
                        edit_add_button->setChecked(false);
                        edit_selection_button->setChecked(false);
                    }
                }
            });

        connect(edit_selection_button, &QPushButton::clicked, [&]
            {
                if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_SELECTION))
                {
                    _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_SELECTION);
                }
                else
                {
                    _mapCreationEditorFlags.set(MapCreationFlags::EDIT_SELECTION);
                    if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_ADD)
                        || _mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_REMOVE)
                        || _mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_MOVE))
                    {
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_REMOVE);
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_ADD);
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_MOVE);
                        edit_remove_button->setChecked(false);
                        edit_add_button->setChecked(false);
                        edit_move_button->setChecked(false);
                    }
                }
            });

        connect(edit_add_button, &QPushButton::clicked, [&]
            {
                if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_ADD))
                {
                    _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_ADD);
                }
                else
                {
                    _mapCreationEditorFlags.set(MapCreationFlags::EDIT_ADD);
                    if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_REMOVE)
                        || _mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_SELECTION)
                        || _mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_MOVE))
                    {
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_REMOVE);
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_SELECTION);
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_MOVE);
                        edit_remove_button->setChecked(false);
                        edit_selection_button->setChecked(false);
                        edit_move_button->setChecked(false);
                    }
                }
            });

        connect(edit_remove_button, &QPushButton::clicked, [&]
            {
                if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_REMOVE))
                {
                    _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_REMOVE);
                }
                else
                {
                    _mapCreationEditorFlags.set(MapCreationFlags::EDIT_REMOVE);
                    if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_ADD)
                        || _mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_SELECTION)
                        || _mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_MOVE))
                    {
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_SELECTION);
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_ADD);
                        _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_MOVE);
                        edit_add_button->setChecked(false);
                        edit_selection_button->setChecked(false);
                        edit_move_button->setChecked(false);
                    }
                }
            });

        connect(edit_group_button, &QPushButton::clicked, [&]
            {
                if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_GROUP))
                {
                    _mapCreationEditorFlags.unSet(MapCreationFlags::EDIT_GROUP);
                }
                else
                {
                    _mapCreationEditorFlags.set(MapCreationFlags::EDIT_GROUP);
                }
            });



        // Right side
        auto layout_right_holder = new QWidget(this);
        layout_right_holder->setMinimumWidth(300);
        layout_right_holder->setMaximumWidth(550);
        auto layout_right = new QVBoxLayout(layout_right_holder);
        layout_right_holder->setLayout(layout_right);
        layout->addWidget(layout_right_holder);

        auto layout_selector_wgt = new QWidget(layout_right_holder);
        auto layout_selector = new QHBoxLayout(layout_selector_wgt);
        layout_selector_wgt->setLayout(layout_selector);

        layout_selector->setAlignment(Qt::AlignLeft);
        layout_right->addWidget(layout_selector_wgt);

        _corpse_map_id = new QComboBox(this);
        _corpse_map_id->addItem("None");
        _corpse_map_id->setItemData(0, QVariant(-1));

        // Fill selector combo
        auto maps = project->ClientDatabase->MapRepository->GetMapList();
        auto count = 1;
        for (auto& map : maps)
        {
            std::string name = map.Name[Locale::enUS];
            int map_id = map.Id;
            int area_type = map.InstanceType;

            if (area_type < 0 || area_type > 4 || !World::IsEditableWorld(map.Directory, map.Id))
                continue;

            _corpse_map_id->addItem(QString::number(map_id) + " - " + QString::fromUtf8(name.c_str()));
            _corpse_map_id->setItemData(count, QVariant(map_id));

            count++;
        }

        auto add_btn = new QPushButton("New", this);
        add_btn->setIcon(Noggit::Ui::FontAwesomeIcon(Noggit::Ui::FontAwesome::plus));
        layout_selector->addWidget(add_btn);

        add_btn->setAccessibleName("map_wizard_add_button");

        auto remove_btn = new QPushButton("Remove", this);
        remove_btn->setIcon(Noggit::Ui::FontAwesomeIcon(Noggit::Ui::FontAwesome::times));
        layout_selector->addWidget(remove_btn);

        remove_btn->setAccessibleName("map_wizard_remove_button");

        _map_settings = new QGroupBox("Map settings", this);
        layout_right->addWidget(_map_settings);


        auto map_settings_layout = new QGridLayout(_map_settings);
        _map_settings->setLayout(map_settings_layout);

        _directory = new QLineEdit(_map_settings);
        map_settings_layout->addWidget(new QLabel("Map directory:"), 0,0);
        map_settings_layout->addWidget(_directory,0,1);

        _is_big_alpha = new QCheckBox(this);
        map_settings_layout->addWidget(new QLabel("Big alpha:"), 1, 0);
        map_settings_layout->addWidget(_is_big_alpha, 1, 1);
        _is_big_alpha->setChecked(true);

        _sort_by_size_cat = new QCheckBox(this);
        map_settings_layout->addWidget(new QLabel("Sort models"), 2, 0);
        map_settings_layout->addWidget(_sort_by_size_cat, 2, 1);
        _sort_by_size_cat->setChecked(true);
        _sort_by_size_cat->setToolTip("Sorts models based on their size on save. May help increase loading speed of the map.");


        _instance_type = new QComboBox(_map_settings);
        _instance_type->addItem("Continent");
        _instance_type->setItemData(0, QVariant(0));

        _instance_type->addItem("Instance");
        _instance_type->setItemData(1, QVariant(1));

        _instance_type->addItem("Raid");
        _instance_type->setItemData(2, QVariant(2));

        _instance_type->addItem("Battleground");
        _instance_type->setItemData(3, QVariant(3));

        _instance_type->addItem("Arena");
        _instance_type->setItemData(4, QVariant(4));

        map_settings_layout->addWidget(new QLabel("Map type:"), 3, 0);
        map_settings_layout->addWidget(_instance_type, 3, 1);

        _map_name = new Widget::LocaleStringWidget(_map_settings);
        map_settings_layout->addWidget(new QLabel("Map name:"), 4, 0);
        map_settings_layout->addWidget(_map_name, 4, 1);

        _area_table_id = new QComboBox(_map_settings);
        map_settings_layout->addWidget(new QLabel("Area ID:"), 5, 0);
        map_settings_layout->addWidget(_area_table_id, 5, 1);

        int counta = 1;
        auto areas = project->ClientDatabase->AreaTableRepository->GetAllAreas();
        _area_table_id->addItem(QString::fromStdString("Continent Fallback"));
        _area_table_id->setItemData(counta, QVariant(0));
        for (auto& area : areas)
        {
            auto areaName = area.AreaName[Locale::enUS];
            _area_table_id->addItem(QString::fromStdString(areaName));
            _area_table_id->setItemData(counta, QVariant(area.Id));
            counta++;
        }
      
        _map_desc_alliance = new Widget::LocaleStringWidget(_map_settings);
        map_settings_layout->addWidget(new QLabel("Description (Alliance):"), 6, 0);
        map_settings_layout->addWidget(_map_desc_alliance, 6, 1);

        _map_desc_horde = new Widget::LocaleStringWidget(_map_settings);
        map_settings_layout->addWidget(new QLabel("Description (Horde):"), 7, 0);
        map_settings_layout->addWidget(_map_desc_horde, 7, 1);

        _loading_screen = new QComboBox(_map_settings);
        auto loadingScreens = project->ClientDatabase->LoadingScreenRepository->GetLoadingScreens();
        int countl = 0;
        for (auto const& loadingscreen : loadingScreens)
        {
            _loading_screen->addItem(QString::fromStdString(loadingscreen.Name));
            _loading_screen->setItemData(countl, QVariant(loadingscreen.Id));
            countl++;
        }

    	map_settings_layout->addWidget(new QLabel("Loading screen:"), 8, 0);
		map_settings_layout->addWidget(_loading_screen, 8, 1);

        _minimap_icon_scale = new QDoubleSpinBox(_map_settings);
        map_settings_layout->addWidget(new QLabel("Minimap icon scale:"), 9, 0);
        map_settings_layout->addWidget(_minimap_icon_scale, 9, 1);

        _corpse_map_id->setCurrentText("None");
        map_settings_layout->addWidget(new QLabel("Corpse map:"), 10, 0);
        map_settings_layout->addWidget(_corpse_map_id, 10, 1);


        _corpse_x = new QDoubleSpinBox(_map_settings);
        map_settings_layout->addWidget(new QLabel("Corpse X:"), 11, 0);
        map_settings_layout->addWidget(_corpse_x, 11, 1);
        _corpse_x->setMinimum(-17066.66656); // map size
        _corpse_x->setMaximum(17066.66656);

        _corpse_y = new QDoubleSpinBox(_map_settings);
        map_settings_layout->addWidget(new QLabel("Corpse Y:"), 12, 0);
        map_settings_layout->addWidget(_corpse_y, 12, 1);
        _corpse_y->setMinimum(-17066.66656); // map size
        _corpse_y->setMaximum(17066.66656);

        _time_of_day_override = new QSpinBox(_map_settings);
        _time_of_day_override->setMinimum(-1);
        _time_of_day_override->setMaximum(2880); // Time Values from 0 to 2880 where each number represents a half minute from midnight to midnight 
        _time_of_day_override->setValue(-1);

        map_settings_layout->addWidget(new QLabel("Daytime override:"), 13, 0);
        map_settings_layout->addWidget(_time_of_day_override, 13, 1);

        _expansion_id = new QComboBox(_map_settings);

        _expansion_id->addItem("Classic");
        _expansion_id->setItemData(0, QVariant(0));

        _expansion_id->addItem("Burning Crusade");
        _expansion_id->setItemData(1, QVariant(1));

        _expansion_id->addItem("Wrath of the Lich King");
        _expansion_id->setItemData(2, QVariant(2));
        map_settings_layout->addWidget(new QLabel("Expansion:"), 14, 0);
        map_settings_layout->addWidget(_expansion_id, 14, 1);

        _raid_offset = new QSpinBox(_map_settings);
        _raid_offset->setMaximum(std::numeric_limits<std::int32_t>::max());
        map_settings_layout->addWidget(new QLabel("Raid offset:"), 15, 0);
        map_settings_layout->addWidget(_raid_offset, 15, 1);

        _max_players = new QSpinBox(_map_settings);
        _max_players->setMaximum(std::numeric_limits<std::int32_t>::max());
        map_settings_layout->addWidget(new QLabel("Max players:"), 16, 0);
        map_settings_layout->addWidget(_max_players, 16, 1);

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
            , [&]()
            {
                saveCurrentEntry();
            });

        connect(discard_btn, &QPushButton::clicked
            , [&]()
            {
                discardChanges();
            });

        connect(add_btn, &QPushButton::clicked
            , [&]()
            {
                addNewMap();
            });

        connect(remove_btn, &QPushButton::clicked
            , [&]()
            {
                removeMap();
            });

        _connection = connect(reinterpret_cast<Noggit::Ui::Windows::NoggitWindow*>(parent),
            QOverload<int>::of(&Noggit::Ui::Windows::NoggitWindow::mapSelected)
            , [&](int index)
            {
                selectMap(index);
            }
        );

        // Selection

        QObject::connect(_minimap_widget, &Noggit::Ui::minimap_widget::tile_clicked
            , [this](QPoint tile)
            {
                if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_MOVE))
                {
                    int x = tile.x();
                    int y = tile.y();

                    auto selectedTiles = _world->mapIndex.getSelectedTiles();
                    auto totalX = 0;
                    auto totalY = 0;
                    auto length = selectedTiles.size();

                    if (length <= 0)
                        return;

                    for (auto const tileEntry : selectedTiles)
                    {
                        auto tileX = tileEntry.first;
                        auto tileY = tileEntry.second;

                        totalX += tileX;
                        totalY += tileY;
                    }

                    auto centerX = totalX / length;
                    auto centerY = totalY / length;

                    for (auto const tileEntry : selectedTiles)
                    {
                        auto tileX = tileEntry.first;
                        auto tileY = tileEntry.second;

                        auto vectorX = centerX - tileX;
                        auto vectorY = centerY - tileY;

                        auto newX = x - vectorX;
                        auto newY = y - vectorY;

                        if (!_world->mapIndex.hasTile(TileIndex(newX, newY)))
                        {
                            _world->mapIndex.removeTile(TileIndex(tileX, tileY));
                            _world->mapIndex.addTile(TileIndex(newX, newY));
                            _world->mapIndex.selectTile(TileIndex(newX, newY));
                        }
                        else
                        {
                            auto tile = _world->mapIndex.getTileEntry(TileIndex(newX, newY));
                            auto tileOld = _world->mapIndex.getTileEntry(TileIndex(tileX, tileY));

                            if (!tile->selected)
                                _world->mapIndex.removeTile(TileIndex(newX, newY));
                            if (!tileOld->selected)
                                _world->mapIndex.removeTile(TileIndex(tileX, tileY));

                            _world->mapIndex.addTile(TileIndex(newX, newY));
                            _world->mapIndex.selectTile(TileIndex(newX, newY));
                        }
                    }
                }
                else if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_GROUP))
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
                                if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_ADD))
                                {
                                    _world->mapIndex.addTile(TileIndex(x_final, y_final));
                                }
                            }
                            else if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_REMOVE))
                            {
                                _world->mapIndex.removeTile(TileIndex(x_final, y_final));
                            }
                            else if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_SELECTION))
                            {
                                _world->mapIndex.selectTile(TileIndex(x_final, y_final));
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
                        if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_ADD))
                        {

                            _world->mapIndex.addTile(TileIndex(x, y));
                        }
                    }
                    else if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_REMOVE))
                    {
                        _world->mapIndex.removeTile(TileIndex(x, y));
                    }
                    else if (_mapCreationEditorFlags.IsSet(MapCreationFlags::EDIT_SELECTION))
                    {
                        _world->mapIndex.selectTile(TileIndex(x, y));
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
        _world = new World(_project, mapEntry.Directory, map_id, Noggit::NoggitRenderContext::MAP_VIEW);
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

        for (int i = 0; i < _area_table_id->count(); ++i)
        {
            auto index = _area_table_id->itemData(i);
            if (index.toInt() == mapEntry.AreaTableId)
            {
                _area_table_id->setCurrentIndex(i);
            }
        }

        for (int i = 0; i < _loading_screen->count(); ++i)
        {
            auto index = _loading_screen->itemData(i);
            if (index.toInt() == mapEntry.LoadingScreenId)
            {
                _loading_screen->setCurrentIndex(i);
            }
        }
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
            }
            else
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
                prompt.setIcon(QMessageBox::Warning);
                prompt.setWindowFlags(Qt::WindowStaysOnTopHint);
                prompt.setText("Warning!");
                prompt.setInformativeText("Saving failed. The map with this directory name already exists. Rename it to save.");
                prompt.addButton("Okay", QMessageBox::AcceptRole);
                prompt.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

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

            auto filepath = project_path / BlizzardArchive::ClientData::normalizeFilenameInternal(filename.str());

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
            newMap.AreaTableId = _area_table_id->itemData(_area_table_id->currentIndex()).toInt();
            newMap.LoadingScreenId = _loading_screen->itemData(_loading_screen->currentIndex()).toInt();
            newMap.CorpseMapId = _corpse_map_id->itemData(_corpse_map_id->currentIndex()).toInt();
            newMap.TimeDayOverride = _time_of_day_override->value();
            newMap.ExpansionId = _expansion_id->itemData(_expansion_id->currentIndex()).toInt();
            newMap.RaidOffset = _raid_offset->value();
            newMap.MaxPlayers = _max_players->value();
            newMap.MiniMapScale = static_cast<float>(_minimap_icon_scale->value());
            newMap.Corpse_X = static_cast<float>(_corpse_x->value());
            newMap.Corpse_Y = static_cast<float>(_corpse_y->value());
            newMap.Flags = _sort_by_size_cat->isChecked() ? 16 : 0;
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
            currentMap.AreaTableId = _area_table_id->itemData(_area_table_id->currentIndex()).toInt();
            currentMap.LoadingScreenId = _loading_screen->itemData(_loading_screen->currentIndex()).toInt();
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

        auto createdMap = _project->ClientDatabase->MapRepository->CreateMap(newMap);
        _cur_map_id = createdMap.Id;

        if (_world)
        {
            delete _world;
        }

        _world = new World(_project, "New_Map", _cur_map_id, Noggit::NoggitRenderContext::MAP_VIEW, true);
        _minimap_widget->world(_world);

        _directory->setText("New_Map");
        _directory->setEnabled(true);

        _is_big_alpha->setChecked(true);
        _is_big_alpha->setEnabled(true);

        _sort_by_size_cat->setChecked(true);

        _instance_type->setCurrentIndex(0);

        _map_name->clear();
        _area_table_id->setCurrentIndex(0);

        _map_desc_alliance->clear();
        _map_desc_horde->clear();


        _loading_screen->setCurrentIndex(0);
        _minimap_icon_scale->setValue(1.0f);

        _corpse_map_id->setCurrentIndex(0);


        _corpse_x->setValue(0.0f);
        _corpse_y->setValue(0.0f);
        _time_of_day_override->setValue(-1);
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

}