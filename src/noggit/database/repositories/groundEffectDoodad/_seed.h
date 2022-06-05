#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class IGroundEffectDoodadRepository
	{
	public:
		virtual std::string GetGroundEffectDoodad(int doodadId) = 0;
	};

	class WotlkGroundEffectDoodadRepository : public IGroundEffectDoodadRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkGroundEffectDoodadRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}

		std::string GetGroundEffectDoodad(int doodadId) override
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT Doodadpath FROM GroundEffectDoodad WHERE ID = ?; ");

			query.bind(1, doodadId);

			if (query.executeStep())
			{
				return query.getColumn(0).getString();
			}
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