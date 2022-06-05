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

		std::string GetSkyboxFilenameById(int id)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT Name FROM LightSkybox WHERE ID = ?; ");

			query.bind(1, id);

			if (query.executeStep())
			{
				return query.getColumn(0).getString();
			}
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