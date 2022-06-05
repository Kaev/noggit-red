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
#include <noggit/database/repositories/areaTable/_seed.h>    
#include <noggit/database/repositories/loadingScreen/_seed.h>
#include <noggit/database/repositories/liquidType/_seed.h>    
#include <noggit/database/repositories/light/_seed.h>    
#include <noggit/database/repositories/lightFloatBand/_seed.h>    
#include <noggit/database/repositories/lightIntBand/_seed.h>    
#include <noggit/database/repositories/lightParams/_seed.h>    
#include <noggit/database/repositories/lightIntBand/_seed.h>    
#include <noggit/database/repositories/lightSkybox/_seed.h>   
#include <noggit/database/repositories/groundEffectDoodad/_seed.h>    
#include <noggit/database/repositories/groundEffectTexture/_seed.h>    
 
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
        //these will become interfaces
        std::shared_ptr<Repositories::WotlkMapRepository> MapRepository;
        std::shared_ptr<Repositories::WotlkLoadingScreenRepository> LoadingScreenRepository;
        std::shared_ptr<Repositories::WotlkAreaTableRepository>AreaTableRepository;
        std::shared_ptr<Repositories::WotlkLiquidTypeRepository> LiquidTypeRepository;
        std::shared_ptr<Repositories::WotlkGroundEffectDoodadRepository> GroundEffectDoodadRepository;
        std::shared_ptr<Repositories::WotlkGroundEffectTextureRepository> GroundEffectTextureRepository;
        std::shared_ptr<Repositories::WotlkLightRepository> LightRepository;
        std::shared_ptr<Repositories::WotlkLightParamsRepository> LightParamsRepository;
        std::shared_ptr<Repositories::WotlkLightSkyBoxRepository> LightSkyBoxRepository;
        std::shared_ptr<Repositories::WotlkLightIntBandRepository> LightIntBandRepository;
        std::shared_ptr<Repositories::WotlkLightFloatBandRepository> LightFloatBandRepository;

        ApplicationProjectRepositories(std::filesystem::path const& database_path)
        {
            MapRepository = std::make_shared<Repositories::WotlkMapRepository>(database_path); 
            LoadingScreenRepository = std::make_shared<Repositories::WotlkLoadingScreenRepository>(database_path);
            AreaTableRepository = std::make_shared<Repositories::WotlkAreaTableRepository>(database_path);
            LiquidTypeRepository = std::make_shared<Repositories::WotlkLiquidTypeRepository>(database_path);
            GroundEffectDoodadRepository = std::make_shared<Repositories::WotlkGroundEffectDoodadRepository>(database_path);
            GroundEffectTextureRepository = std::make_shared<Repositories::WotlkGroundEffectTextureRepository>(database_path);
            LightRepository = std::make_shared<Repositories::WotlkLightRepository>(database_path);
            LightParamsRepository = std::make_shared<Repositories::WotlkLightParamsRepository>(database_path);
            LightSkyBoxRepository = std::make_shared<Repositories::WotlkLightSkyBoxRepository>(database_path);
            LightIntBandRepository = std::make_shared<Repositories::WotlkLightIntBandRepository>(database_path);
            LightFloatBandRepository = std::make_shared<Repositories::WotlkLightFloatBandRepository>(database_path);
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