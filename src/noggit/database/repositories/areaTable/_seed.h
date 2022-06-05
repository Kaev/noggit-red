#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class IAreaTableRepository
	{

	};

	class WotlkAreaTableRepository : public IAreaTableRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkAreaTableRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};

	class ShadowlandsAreaTableRepository : public IAreaTableRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsAreaTableRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}