//Folder to contain all of the project related files
#pragma once

#include <map>
#include <memory>
#include <blizzard-archive-library/include/CASCArchive.hpp>
#include <blizzard-archive-library/include/ClientFile.hpp>
#include <blizzard-archive-library/include/Exception.hpp>
#include <blizzard-database-library/include/BlizzardDatabase.h>
#include <noggit/application/Configuration/NoggitApplicationConfiguration.hpp>
#include <noggit/ui/windows/downloadFileDialog/DownloadFileDialog.h>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonObject>
#include <QFile>
#include <filesystem>
#include <fstream>
#include <vector>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QFile>
#include <QString>
#include <QObject>
#include <QString>
#include <thread>
#include <chrono>
#include <cassert>
#include <glm/vec3.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/types/NoggitTypes.h>
#include <noggit/database/NoggitDatabaseCreator.h>
#include <noggit/database/repositories/schema/_seed.h>
#include "ApplicationProjectReader.h"
#include "ApplicationProjectWriter.h"

namespace Noggit::Project
{
  struct ClientVersionFactory
  {
     
    static ProjectVersion mapToEnumVersion(std::string const& projectVersion)
    {
      if (projectVersion == "Wrath Of The Lich King")
        return ProjectVersion::WOTLK;
      if (projectVersion == "Shadowlands")
        return ProjectVersion::SL;

      assert(false);
    }

    static ProjectBuildInformation mapToProjectBuildInformation(std::string const& projectVersion)
    {
        auto version = mapToEnumVersion(projectVersion);
        auto buildInformation = ProjectBuildInformation();
        buildInformation.Version = version;

        if (version == ProjectVersion::WOTLK)
        {
            buildInformation.Expansion = 3;
            buildInformation.Major = 3;
            buildInformation.Minor = 5;
            buildInformation.Patch = 12340;   
        }

        if (version == ProjectVersion::SL)
        {
            buildInformation.Expansion = 9;
            buildInformation.Major = 1;
            buildInformation.Minor = 0;
            buildInformation.Patch = 39584;
        }

        return buildInformation;
    }

    static std::string MapToStringVersion(ProjectVersion const& projectVersion)
    {
      if (projectVersion == ProjectVersion::WOTLK)
        return std::string("Wrath Of The Lich King");
      if (projectVersion == ProjectVersion::SL)
        return std::string("Shadowlands");

      assert(false);
    }
  };

  struct NoggitProjectBookmarkMap
  {
    int map_id;
    std::string name;
    glm::vec3 position;
    float camera_yaw;
    float camera_pitch;
  };

  struct NoggitProjectPinnedMap
  {
    int MapId;
    std::string MapName;
  };

  class NoggitProject
  {
    std::shared_ptr<ApplicationProjectWriter> _projectWriter;
  public:
    std::string ProjectPath;
    std::string ProjectName;
    std::string ClientPath;
    std::string databasePath;
    ProjectVersion projectVersion;
    std::vector<NoggitProjectPinnedMap> PinnedMaps;
    std::vector<NoggitProjectBookmarkMap> Bookmarks;
    std::shared_ptr<Database::ApplicationProjectRepositories> ClientDatabase;
    std::shared_ptr<BlizzardArchive::ClientData> ClientData;

    NoggitProject()
    {
      PinnedMaps = std::vector<NoggitProjectPinnedMap>();
      Bookmarks = std::vector<NoggitProjectBookmarkMap>();
      _projectWriter = std::make_shared<ApplicationProjectWriter>();
    }

    void createBookmark(const NoggitProjectBookmarkMap& bookmark)
    {
      Bookmarks.push_back(bookmark);

      _projectWriter->saveProject(this, std::filesystem::path(ProjectPath));
    }

    void deleteBookmark()
    {

    }

    void pinMap(int map_id, const std::string& map_name)
    {
      auto pinnedMap = NoggitProjectPinnedMap();
      pinnedMap.MapName = map_name;
      pinnedMap.MapId = map_id;

      auto pinnedMapFound = std::find_if(std::begin(PinnedMaps), std::end(PinnedMaps),
                                         [&](Project::NoggitProjectPinnedMap pinnedMap)
                                         {
                                           return pinnedMap.MapId == map_id;
                                         });

      if (pinnedMapFound != std::end(PinnedMaps))
        return;

      PinnedMaps.push_back(pinnedMap);

      _projectWriter->saveProject(this, std::filesystem::path(ProjectPath));
    }

    void unpinMap(int mapId)
    {
      PinnedMaps.erase(std::remove_if(PinnedMaps.begin(), PinnedMaps.end(),
                                      [=](NoggitProjectPinnedMap pinnedMap)
                                      {
                                        return pinnedMap.MapId == mapId;
                                      }),
                       PinnedMaps.end());

      _projectWriter->saveProject(this, std::filesystem::path(ProjectPath));
    }
  };

  class ApplicationProject
  {
    std::shared_ptr<NoggitProject> _active_project;
    std::shared_ptr<Application::NoggitApplicationConfiguration> _configuration;
  public:
    ApplicationProject(std::shared_ptr<Application::NoggitApplicationConfiguration> configuration)
    {
      _active_project = nullptr;
      _configuration = configuration;
    }

    void createProject(std::filesystem::path const& project_path, std::filesystem::path const& client_path, std::filesystem::path const& database_path,
                       std::string const& client_version, std::string const& project_name)
    {
      if (!std::filesystem::exists(project_path))
        std::filesystem::create_directory(project_path);

      auto project = NoggitProject();
      project.ProjectName = project_name;
      project.projectVersion = ClientVersionFactory::mapToEnumVersion(client_version);
      project.ClientPath = client_path.generic_string();
      project.ProjectPath = project_path.generic_string();
      project.databasePath = database_path.generic_string();

      auto project_writer = ApplicationProjectWriter();
      project_writer.saveProject(&project, project_path);

    }

    std::shared_ptr<NoggitProject> loadProject(std::filesystem::path const& project_path)
    {
      ApplicationProjectReader project_reader{};
      auto project = project_reader.readProject(project_path);

      BlizzardDatabaseLib::Structures::Build client_build("3.3.5.12340");
      auto client_archive_version = BlizzardArchive::ClientVersion::WOTLK;
      auto client_archive_locale = BlizzardArchive::Locale::AUTO;
      if (project->projectVersion == ProjectVersion::SL)
      {
          client_archive_version = BlizzardArchive::ClientVersion::SL;
          client_build = BlizzardDatabaseLib::Structures::Build("9.1.0.39584");
          client_archive_locale = BlizzardArchive::Locale::enUS;
      }

      if(!project.has_value())
        return {};

      //Wotlk only right now
      auto projectBuildInforamtion = Noggit::ProjectBuildInformation();
      projectBuildInforamtion.Expansion = 3;
      projectBuildInforamtion.Major = 3;
      projectBuildInforamtion.Minor = 5;
      projectBuildInforamtion.Patch = 12340;
      projectBuildInforamtion.Version = ProjectVersion::WOTLK;

      auto databasePath = project_path / "database.db";
      //If old project with no database, build one
      if (project.value().databasePath == "")
      {
          auto databaseCreator = Noggit::Database::ApplicationProjectDatabase(_configuration, Noggit::Tables);
          databaseCreator.CreateDatabase(databasePath, project_path, project->ClientPath, projectBuildInforamtion);

          auto project_writer = ApplicationProjectWriter();
          project.value().databasePath = databasePath.generic_string();
          project_writer.saveProject(&project.value(), project_path);
      }

      //if database is missing Table load table in
      auto schemaRepository = new Noggit::Database::Repositories::SchemaRepository(databasePath);
      auto missingTables = schemaRepository->GetMissingTables(Noggit::Tables);
      if (missingTables.size() > 0)
      {
          auto databaseCreator = Noggit::Database::ApplicationProjectDatabase(_configuration, missingTables);
          databaseCreator.CreateDatabase(databasePath, project_path, project->ClientPath, projectBuildInforamtion);
      }

      std::string dbd_file_directory = _configuration->ApplicationDatabaseDefinitionsPath;

      auto databaseCreator = std::make_shared<Database::ApplicationProjectDatabase>(_configuration, missingTables);
      project->ClientDatabase = std::make_shared<Database::ApplicationProjectRepositories>(databaseCreator,databasePath);

      try
      {
        project->ClientData = std::make_shared<BlizzardArchive::ClientData>(
            project->ClientPath, client_archive_version, client_archive_locale, project_path.generic_string());
      }
      catch (BlizzardArchive::Exceptions::Locale::LocaleNotFoundError& e)
      {
        QMessageBox::critical(nullptr, "Error", "The client does not appear to be valid.");
        return {};
      }

      return std::make_shared<NoggitProject>(project.value());
    }
  };
}
