#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class ILightFloatBandRepository
	{

	};

	class WotlkLightFloatBandRepository : public ILightFloatBandRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLightFloatBandRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};

	class ShadowlandsLightFloatBandRepository : public ILightFloatBandRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLightFloatBandRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}