#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class ILightRepository
	{

	};

	class WotlkLightRepository : public ILightRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLightRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};

	class ShadowlandsLightRepository : public ILightRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLightRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}