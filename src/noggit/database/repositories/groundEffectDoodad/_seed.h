#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class IGroundEffectDoodadRepository
	{

	};

	class WotlkGroundEffectDoodadRepository : public IGroundEffectDoodadRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkGroundEffectDoodadRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};

	class ShadowlandsGroundEffectDoodadRepository : public IGroundEffectDoodadRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsGroundEffectDoodadRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}