// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/SettingsPanel.h>
#include <noggit/Log.h>

#include <noggit/TextureManager.h>
#include <util/qt/overload.hpp>

#include <boost/format.hpp>

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include <QDir>
#include <QApplication>

#include <ui_SettingsPanel.h>


#include <algorithm>


namespace noggit
{
  namespace ui
  {
    settings::settings(QWidget *parent)
        : QDialog(parent), _settings(new QSettings(this))
    {
      SettingsPanelUi = new Ui::SettingsPanel;
      SettingsPanelUi->setupUi(this);

      connect(SettingsPanelUi->gamePathField, &QLineEdit::textChanged, [&](QString value)
              {
                _settings->setValue("project/game_path", value);
              }
      );


      connect(SettingsPanelUi->gamePathField_browse, &QPushButton::clicked, [=]
              {
                auto result(QFileDialog::getExistingDirectory(
                    nullptr, "WoW Client Path", SettingsPanelUi->gamePathField->text()));

                if (!result.isNull())
                {
                  if (!(result.endsWith("/") || result.endsWith("\\")))
                  {
                    result += "/";
                  }

                  SettingsPanelUi->gamePathField->setText(result);
                }
              }
      );

      connect(SettingsPanelUi->projectPathField, &QLineEdit::textChanged, [&](QString value)
              {
                _settings->setValue("project/path", value);
              }
      );


      connect(SettingsPanelUi->projectPathField_browse, &QPushButton::clicked, [=]
              {
                auto result(QFileDialog::getExistingDirectory(
                    nullptr, "Project Path", SettingsPanelUi->projectPathField->text()));

                if (!result.isNull())
                {
                  if (!(result.endsWith("/") || result.endsWith("\\")))
                  {
                    result += "/";
                  }

                  SettingsPanelUi->projectPathField->setText(result);
                }
              }
      );

      connect(SettingsPanelUi->importPathField, &QLineEdit::textChanged, [&](QString value)
              {
                _settings->setValue("project/import_file", value);
              }
      );


      connect(SettingsPanelUi->importPathField_browse, &QPushButton::clicked, [=]
              {
                auto result(QFileDialog::getOpenFileName(
                    nullptr, "Import File Path", SettingsPanelUi->importPathField->text()));

                if (!result.isNull())
                {
                  SettingsPanelUi->importPathField->setText(result);
                }
              }
      );

      connect(SettingsPanelUi->wmvLogPathField, &QLineEdit::textChanged, [&](QString value)
              {
                _settings->setValue("project/import_file", value);
              }
      );


      connect(SettingsPanelUi->wmvLogPathField_browse, &QPushButton::clicked, [=]
              {
                auto result(QFileDialog::getOpenFileName(
                    nullptr, "WMV Log Path", SettingsPanelUi->wmvLogPathField->text()));

                if (!result.isNull())
                {
                  SettingsPanelUi->wmvLogPathField->setText(result);
                }
              }
      );


#ifdef USE_MYSQL_UID_STORAGE
      SettingsPanelUi->MySQL_box->setEnabled(true);
      SettingsPanelUi->mysql_warning->setVisible(false);
#endif

      SettingsPanelUi->_theme->addItem("System");

      QDir theme_dir = QDir("./themes/");
      if (theme_dir.exists())
      {
        for (auto dir : theme_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
        {
          if (QDir(theme_dir.path() + "/" + dir).exists("theme.qss"))
          {
            SettingsPanelUi->_theme->addItem(dir);
          }
        }
      } else
      {
        LogError
            << "Failed to load themes. The \"themes/\" folder does not exist in Noggit directory. Using system theme."
            << std::endl;
      }

      connect(SettingsPanelUi->_theme, &QComboBox::currentTextChanged, [&](QString s)
              {
                if (s == "System")
                {
                  qApp->setStyleSheet("");
                  return;
                }

                QFile file((boost::format("./themes/%s/theme.qss") % s.toStdString().c_str()).str().c_str());
                if (file.open(QFile::ReadOnly))
                {
                  QString style_sheet = QLatin1String(file.readAll());
                  qApp->setStyleSheet(style_sheet);;
                }
              }
      );

      SettingsPanelUi->_wireframe_color->setColor(Qt::white);

      connect(SettingsPanelUi->saveButton, &QPushButton::clicked, [this]
              {
                hide();
                save_changes();
              }
      );

      connect(SettingsPanelUi->discardButton, &QPushButton::clicked, [this]
              {
                hide();
                discard_changes();
              }
      );

      // load the values in the fields
      discard_changes();
    }

    void settings::discard_changes()
    {
      SettingsPanelUi->gamePathField->setText(_settings->value("project/game_path").toString());
      SettingsPanelUi->projectPathField->setText(_settings->value("project/path").toString());
      SettingsPanelUi->importPathField->setText(_settings->value("project/import_file", "import.txt").toString());
      SettingsPanelUi->wmvLogPathField->setText(_settings->value("project/wmv_log_file").toString());
      SettingsPanelUi->viewDistanceField->setValue(_settings->value("view_distance", 1000.f).toFloat());
      SettingsPanelUi->farZField->setValue(_settings->value("farZ", 2048.f).toFloat());
      SettingsPanelUi->tabletModeCheck->setChecked(_settings->value("tablet/enabled", false).toBool());
      SettingsPanelUi->_undock_tool_properties->setChecked(
          _settings->value("undock_tool_properties/enabled", true).toBool());
      SettingsPanelUi->_undock_small_texture_palette->setChecked(
          _settings->value("undock_small_texture_palette/enabled", true).toBool());
      SettingsPanelUi->_vsync_cb->setChecked(_settings->value("vsync", false).toBool());
      SettingsPanelUi->_anti_aliasing_cb->setChecked(_settings->value("anti_aliasing", false).toBool());
      SettingsPanelUi->_fullscreen_cb->setChecked(_settings->value("fullscreen", false).toBool());
      SettingsPanelUi->_adt_unload_dist->setValue(_settings->value("unload_dist", 5).toInt());
      SettingsPanelUi->_adt_unload_check_interval->setValue(_settings->value("unload_interval", 5).toInt());
      SettingsPanelUi->_uid_cb->setChecked(_settings->value("uid_startup_check", true).toBool());
      SettingsPanelUi->_additional_file_loading_log->setChecked(
          _settings->value("additional_file_loading_log", false).toBool());
      SettingsPanelUi->_theme->setCurrentText(_settings->value("theme", "Dark").toString());

#ifdef USE_MYSQL_UID_STORAGE
      SettingsPanelUi->_mysql_box->setChecked (_settings->value ("project/mysql/enabled").toBool());
      SettingsPanelUi->_mysql_server_field->setText (_settings->value ("project/mysql/server").toString());
      SettingsPanelUi->_mysql_user_field->setText(_settings->value ("project/mysql/user").toString());
      SettingsPanelUi->_mysql_pwd_field->setText (_settings->value ("project/mysql/pwd").toString());
      SettingsPanelUi->_mysql_db_field->setText (_settings->value ("project/mysql/db").toString());
#endif

      //SettingsPanelUi->_wireframe_type_group->button(_settings->value("wireframe/type", 0).toInt())->toggle();
      SettingsPanelUi->_wireframe_radius->setValue(_settings->value("wireframe/radius", 1.5f).toFloat());
      SettingsPanelUi->_wireframe_width->setValue(_settings->value("wireframe/width", 1.f).toFloat());
      SettingsPanelUi->_wireframe_color->setColor(_settings->value("wireframe/color").value<QColor>());
    }

    void settings::save_changes()
    {
      _settings->setValue("project/game_path", SettingsPanelUi->gamePathField->text());
      _settings->setValue("project/path", SettingsPanelUi->projectPathField->text());
      _settings->setValue("project/import_file", SettingsPanelUi->importPathField->text());
      _settings->setValue("project/wmv_log_file", SettingsPanelUi->wmvLogPathField->text());
      _settings->setValue("farZ", SettingsPanelUi->farZField->value());
      _settings->setValue("view_distance", SettingsPanelUi->viewDistanceField->value());
      _settings->setValue("tablet/enabled", SettingsPanelUi->tabletModeCheck->isChecked());
      _settings->setValue("undock_tool_properties/enabled", SettingsPanelUi->_undock_tool_properties->isChecked());
      _settings->setValue("undock_small_texture_palette/enabled",
                          SettingsPanelUi->_undock_small_texture_palette->isChecked());
      _settings->setValue("vsync", SettingsPanelUi->_vsync_cb->isChecked());
      _settings->setValue("anti_aliasing", SettingsPanelUi->_anti_aliasing_cb->isChecked());
      _settings->setValue("fullscreen", SettingsPanelUi->_fullscreen_cb->isChecked());
      _settings->setValue("unload_dist", SettingsPanelUi->_adt_unload_dist->value());
      _settings->setValue("unload_interval", SettingsPanelUi->_adt_unload_check_interval->value());
      _settings->setValue("uid_startup_check", SettingsPanelUi->_uid_cb->isChecked());
      _settings->setValue("additional_file_loading_log", SettingsPanelUi->_additional_file_loading_log->isChecked());

#ifdef USE_MYSQL_UID_STORAGE
      _settings->setValue ("project/mysql/enabled", _mysql_box->isChecked());
      _settings->setValue ("project/mysql/server", _mysql_server_field->text());
      _settings->setValue ("project/mysql/user", _mysql_user_field->text());
      _settings->setValue ("project/mysql/pwd", _mysql_pwd_field->text());
      _settings->setValue ("project/mysql/db", _mysql_db_field->text());
#endif

      _settings->setValue("wireframe/type", SettingsPanelUi->_wireframe_type_group->checkedId());
      _settings->setValue("wireframe/radius", SettingsPanelUi->_wireframe_radius->value());
      _settings->setValue("wireframe/width", SettingsPanelUi->_wireframe_width->value());
      _settings->setValue("wireframe/color", SettingsPanelUi->_wireframe_color->color());
      _settings->setValue("theme", SettingsPanelUi->_theme->currentText());

      _settings->sync();
    }
  }
}