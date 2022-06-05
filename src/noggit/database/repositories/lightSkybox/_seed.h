#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class ILightSkyBoxRepository
	{

	};

	class WotlkLightSkyBoxRepository : public ILightSkyBoxRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLightSkyBoxRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};

	class ShadowlandsLightSkyBoxRepository : public ILightSkyBoxRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLightSkyBoxRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}