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

#include "ApplicationProjectReader.h"
#include "ApplicationProjectWriter.h"

namespace Noggit::Project
{
  enum class ProjectVersion
  {
    VANILLA,
    BC,
    WOTLK,
    CATA,
    PANDARIA,
    WOD,
    LEGION,
    BFA,
    SL
  };

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
    ProjectVersion projectVersion;
    std::vector<NoggitProjectPinnedMap> PinnedMaps;
    std::vector<NoggitProjectBookmarkMap> Bookmarks;
    std::shared_ptr<BlizzardDatabaseLib::BlizzardDatabase> ClientDatabase;
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

    void createProject(std::filesystem::path const& project_path, std::filesystem::path const& client_path,
                       std::string const& client_version, std::string const& project_name)
    {
      if (!std::filesystem::exists(project_path))
        std::filesystem::create_directory(project_path);

      auto project = NoggitProject();
      project.ProjectName = project_name;
      project.projectVersion = ClientVersionFactory::mapToEnumVersion(client_version);
      project.ClientPath = client_path.generic_string();
      project.ProjectPath = project_path.generic_string();

      auto project_writer = ApplicationProjectWriter();
      project_writer.saveProject(&project, project_path);


    }

    std::shared_ptr<NoggitProject> loadProject(std::filesystem::path const& project_path)
    {
      ApplicationProjectReader project_reader{};
      auto project = project_reader.readProject(project_path);

      assert(project.has_value());

      std::string dbd_file_directory = _configuration->ApplicationDatabaseDefinitionsPath;

      BlizzardDatabaseLib::Structures::Build client_build("3.3.5.12340");
      auto client_archive_version = BlizzardArchive::ClientVersion::WOTLK;
      auto client_archive_locale = BlizzardArchive::Locale::AUTO;
      if (project->projectVersion == ProjectVersion::SL)
      {
        client_archive_version = BlizzardArchive::ClientVersion::SL;
        client_build = BlizzardDatabaseLib::Structures::Build("9.1.0.39584");
        client_archive_locale = BlizzardArchive::Locale::enUS;
      }

      if (project->projectVersion == ProjectVersion::WOTLK)
      {
        client_archive_version = BlizzardArchive::ClientVersion::WOTLK;
        client_build = BlizzardDatabaseLib::Structures::Build("3.3.5.12340");
        client_archive_locale = BlizzardArchive::Locale::AUTO;
      }

      project->ClientDatabase = std::make_shared<BlizzardDatabaseLib::BlizzardDatabase>(dbd_file_directory, client_build);

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
