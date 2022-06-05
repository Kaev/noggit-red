#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class ILightIntBandRepository
	{

	};

	class WotlkLightIntBandRepository : public ILightIntBandRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLightIntBandRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};

	class ShadowlandsLightIntBandRepository : public ILightIntBandRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLightIntBandRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}