// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once
#include <QtWidgets/QDialog>
#include <QObject>
#include <QFile>
#include <future>
#include <QThread>
#include <QMainWindow>
#include <ui_LoadingScreenViewer.h>

#include "noggit/project/ApplicationProject.h"
#include "opengl/context.hpp"

namespace Noggit::Ui::Windows
{
	class LoadingScreenViewer : public QMainWindow
	{
          Q_OBJECT
              QThread workerThread;
      public:
          LoadingScreenViewer(std::shared_ptr<Project::NoggitProject> project, std::string loadingScreenPath, QWidget* parent = nullptr);
        
      private:
          ::Ui::LoadingScreenViewer* ui;
          std::string _filePath;
          std::shared_ptr<Project::NoggitProject> _project;
	};
}
