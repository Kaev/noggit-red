#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class ILightParamsRepository
	{

	};

	class WotlkLightParamsRepository : public ILightParamsRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLightParamsRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};

	class ShadowlandsLightParamsRepository : public ILightParamsRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLightParamsRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}