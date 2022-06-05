#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class ILiquidTypeRepository
	{

	};

	class WotlkLiquidTypeRepository : public ILiquidTypeRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLiquidTypeRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};

	class ShadowlandsLiquidTypeRepository : public ILiquidTypeRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLiquidTypeRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}