// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "CreateProjectComponent.hpp"
#include <noggit/ui/windows/projectSelection/components/RecentProjectsComponent.hpp>

using namespace Noggit::Ui::Component;

void CreateProjectComponent::createProject(Noggit::Ui::Windows::NoggitProjectSelectionWindow* parent
                                           , ProjectInformation& project_information)
{
  auto application_configuration = parent->_noggit_application->getConfiguration();
  auto application_project_service = Noggit::Project::ApplicationProject(application_configuration);

  if (std::filesystem::exists(project_information.project_path))
  {
    for (const auto& entry : std::filesystem::directory_iterator(project_information.project_path))
    {
      if (entry.path().extension() == ".noggitproj")
      {
        QMessageBox::critical(parent, "Error", "Selected already contains an existing project.");
        return;
      }
    }
  }

  auto projectDatabasePath = project_information.project_path + "\\database.db";
  if (!std::filesystem::exists(projectDatabasePath))
  {
      auto buildInformation = Noggit::Project::ClientVersionFactory::mapToProjectBuildInformation(project_information.game_client_version);
      auto databaseCreator = Noggit::Database::ApplicationProjectDatabase(application_configuration, Noggit::Tables);

      databaseCreator.CreateDatabase(projectDatabasePath,
          project_information.project_path,
          project_information.game_client_path,
          buildInformation);

      //Only here for debugging
      //databaseCreator.ExportDatabase(projectDatabasePath,
      //    project_information.project_path,
      //    project_information.game_client_path,
      //    buildInformation);
  }

  application_project_service.createProject(project_information.project_path,
                                            project_information.game_client_path,
                                 projectDatabasePath,
                                            project_information.game_client_version,
                                            project_information.project_name);

  RecentProjectsComponent::registerProjectChange(project_information.project_path);

}