#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	struct LoadingScreenEntry
	{
		int Id;
		std::string Name;
		std::string FileName;
		bool HasWideScreen;
	};

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

		std::vector<LoadingScreenEntry> GetLoadingScreens()
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT ID, Name, FileName, HasWideScreen FROM LoadingScreens;");

			auto loadingScreens = std::vector<LoadingScreenEntry>();
			while (query.executeStep())
			{
				auto loadingScreen = LoadingScreenEntry();
				loadingScreen.Id = query.getColumn(0).getInt();
				loadingScreen.Name = query.getColumn(1).getString();
				loadingScreen.FileName = query.getColumn(2).getString();
				loadingScreen.HasWideScreen = static_cast<bool>(query.getColumn(3).getInt());

				loadingScreens.push_back(loadingScreen);
			}

			return loadingScreens;
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