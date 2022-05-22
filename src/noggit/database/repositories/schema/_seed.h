#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class SchemaRepository
	{
		std::filesystem::path _databasePath;
	public:
		SchemaRepository(const std::filesystem::path& database_path) : _databasePath(database_path){}

		std::vector<std::string> GetMissingTables(std::vector<std::string> tableList)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT table_name FROM _tables;");

			auto existsingTables = std::vector<std::string>();
			auto missingTables = std::vector<std::string>();
			while (query.executeStep())
			{
				auto tableName = query.getColumn(0).getString();
				existsingTables.push_back(tableName);
			}

			for (auto const& tableName : tableList)
			{
				if (!(std::find(existsingTables.begin(), existsingTables.end(), tableName) != existsingTables.end()))
				{
					missingTables.push_back(tableName);
				}
			}

			return missingTables;
		}
	};
}