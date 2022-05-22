#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class IRepository
	{

	};

	class IMapRepository :public IRepository
	{
	public:

	};

	struct LocaleString
	{
		int id;
		std::map<std::string, std::string> values;
		uint32_t flags;

		const std::string& operator[] (const char* locale)
		{
			return values[std::string(locale)];
		}
	};

	struct MapEntry
	{
		LocaleString map_name;
		std::string file_path;
		int map_id;
		int map_type_id;
		int expansion_id;
		bool pinned;
	};

	class WotlkMapRepository : IMapRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkMapRepository(const std::filesystem::path& database_path): _databasePath(database_path)
		{
		
		}

		LocaleString GetLocaleString(int id)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  selectLocaleStringQuery(db, "SELECT enUS, koKR, frFR, deDE, zhCN, zhTW, esES, esMX, ruRU,"
																   "jaJP, ptPT, itIT, Unk1, Unk2, Unk3,Unk4, flags "
																   "FROM _localeStrings WHERE id = ?");

			selectLocaleStringQuery.bind(1, id);

			auto mapList = std::vector<MapEntry>();
			if(selectLocaleStringQuery.executeStep())
			{
				auto localeString = LocaleString();
				localeString.id = id;
				localeString.values = std::map<std::string, std::string>();

				for (int localeIndex = 0; localeIndex < 16; localeIndex++)
				{
					auto localeValue = selectLocaleStringQuery.getColumn(localeIndex).getString();
					localeString.values.emplace(LocaleNames[localeIndex], localeValue);
				}

				localeString.flags = selectLocaleStringQuery.getColumn(16).getUInt();

				return localeString;
			}
		}

		std::vector<MapEntry> GetMapList()
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT map.ID, map.MapName_lang, map.Directory ,map.ExpansionID, map.InstanceType FROM map WHERE map.InstanceType < 5");

			auto mapList = std::vector<MapEntry>();
			while(query.executeStep())
			{
				auto mapEntry = MapEntry();
				mapEntry.map_id = query.getColumn(0).getInt();
				mapEntry.file_path = query.getColumn(2).getString();
				mapEntry.expansion_id = query.getColumn(3).getInt();
				mapEntry.map_type_id = query.getColumn(4).getInt();

				auto localeStringId = query.getColumn(1).getInt();
				mapEntry.map_name = GetLocaleString(localeStringId);

				mapList.push_back(mapEntry);
			}
			return mapList;
		}

		std::string GetMapNameById(int mapId)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT MapName_lang_0 FROM map WHERE ID = ?; ");

			query.bind(1, mapId);

			if (query.executeStep())
				return query.getColumn(0).getString();
		}

		int GetMapIdByName(const std::string& mapName)
		{		
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT ID FROM map WHERE MapName_lang = '?'");

			query.bind(1, mapName);

			if (query.executeStep())
				return query.getColumn(0).getInt();
		}
	};

	class ShadowlandsMapRepository : IMapRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsMapRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}