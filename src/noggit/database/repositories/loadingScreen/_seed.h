#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class ILoadingScreenRepository
	{

	};

	class WotlkLoadingScreenRepository : public ILoadingScreenRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLoadingScreenRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};

	class ShadowlandsScreenRepository : public ILoadingScreenRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsScreenRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}