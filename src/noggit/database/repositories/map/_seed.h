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

	struct MapEntry
	{
		int Id;
		std::string Directory;
		LocaleString Name;		
		int InstanceType;
		int ExpansionId;
		int Flags;
		int PVP;
		int AreaTableId;
		LocaleString HordeMapDescription;
		LocaleString AllianceMapDescription;
		int LoadingScreenId;
		double MiniMapScale;
		int CorpseMapId;
		double Corpse_X;
		double Corpse_Y;
		int TimeDayOverride;
		int RaidOffset;
		int MaxPlayers;
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
				mapEntry.Id = query.getColumn(0).getInt();
				mapEntry.Directory = query.getColumn(2).getString();
				mapEntry.ExpansionId = query.getColumn(3).getInt();
				mapEntry.InstanceType = query.getColumn(4).getInt();

				auto localeStringId = query.getColumn(1).getInt();
				mapEntry.Name = GetLocaleString(localeStringId);

				mapList.push_back(mapEntry);
			}
			return mapList;
		}

		std::string GetMapDirectory(int mapId)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT Directory FROM map WHERE ID = ?; ");

			query.bind(1, mapId);

			if (query.executeStep())
			{
				return query.getColumn(0).getString();
			}
			
		}

		MapEntry GetMapById(int id)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT Directory, InstanceType, Flags, PVP, MapName_lang,AreaTableID, MapDescription0_lang,"
										 "MapDescription1_lang, LoadingScreenID,MinimapIconScale, CorpseMapID, Corpse_0, Corpse_1,"
										 "TimeOfDayOverride,ExpansionID, RaidOffset, MaxPlayers FROM Map WHERE ID = ? ");

			query.bind(1, id);

			if (query.executeStep())
			{
				auto mapEntry = MapEntry();
				mapEntry.Id = id;
				mapEntry.Directory = query.getColumn(0).getString();
				mapEntry.InstanceType = query.getColumn(1).getInt();
				mapEntry.Flags = query.getColumn(2).getInt();
				mapEntry.PVP = query.getColumn(3).getInt();
				auto mapLangLocaleStringId = query.getColumn(4).getInt();
				mapEntry.Name = GetLocaleString(mapLangLocaleStringId);
				mapEntry.AreaTableId = query.getColumn(5).getInt();
				auto mapHordeDescLocaleStringId = query.getColumn(6).getInt();
				mapEntry.Name = GetLocaleString(mapHordeDescLocaleStringId);
				auto mapAllianceDescLocaleStringId = query.getColumn(7).getInt();
				mapEntry.Name = GetLocaleString(mapAllianceDescLocaleStringId);
				mapEntry.LoadingScreenId = query.getColumn(8).getInt();
				mapEntry.MiniMapScale = query.getColumn(9).getDouble();
				mapEntry.CorpseMapId = query.getColumn(10).getInt();
				mapEntry.Corpse_X = query.getColumn(11).getDouble();
				mapEntry.Corpse_Y = query.getColumn(12).getDouble();
				mapEntry.TimeDayOverride = query.getColumn(13).getInt();
				mapEntry.ExpansionId = query.getColumn(14).getInt();
				mapEntry.RaidOffset = query.getColumn(15).getInt();
				mapEntry.MaxPlayers = query.getColumn(16).getInt();

				return mapEntry;
			}
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