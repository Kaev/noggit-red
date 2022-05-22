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

#include <noggit/database/repositories/map/_seed.h>    

namespace Noggit::Database
{
    enum class TableColumnFlags : int
    {
        None = 0,
        IsId = 1,
        IsArray = 2,
    };

    inline TableColumnFlags operator|(TableColumnFlags a, TableColumnFlags b)
    {
        return static_cast<TableColumnFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline bool operator&(TableColumnFlags a, TableColumnFlags b)
    {
        return static_cast<int>(a) & static_cast<int>(b);
    }

    struct DatabaseColumnSchema
    {
        int ColumnId;
        std::string ColumnName;
        std::string ColumnType;
        TableColumnFlags ColumnFlags;
        int ColumnArrayLength;
    };

    struct ApplicationProjectRepositories
    {
        std::shared_ptr<Repositories::WotlkMapRepository> MapRepository;


        ApplicationProjectRepositories(std::filesystem::path const& database_path)
        {
            MapRepository = std::make_shared<Repositories::WotlkMapRepository>(database_path);
        }
    };

    class ApplicationProjectDatabase
    {
        std::shared_ptr<Application::NoggitApplicationConfiguration> _configuration;
        std::vector<std::string> _tables;   
    public:
        ApplicationProjectDatabase(std::shared_ptr<Application::NoggitApplicationConfiguration> configuration, std::vector<std::string> tables);

        void CreateDatabase(std::filesystem::path const& database_path, std::filesystem::path const& project_path,
            std::filesystem::path const& client_path, ProjectBuildInformation& buildInformation);

        void ExportDatabase(std::filesystem::path const& database_path, std::filesystem::path const& project_path,
            std::filesystem::path const& client_path, ProjectBuildInformation& buildInformation);

        std::string MapDefinitionTypeToDatabaseType(std::string definitionType)
        {
            if (definitionType == "int")
                return "INTEGER";
            else if (definitionType == "float")
                return "REAL";
            else if (definitionType == "string")
                return "TEXT";
            else if (definitionType == "locstring")
                return "INTEGER";
            else
                return "TEXT";
        }
    };
}