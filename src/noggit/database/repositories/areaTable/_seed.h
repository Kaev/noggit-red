#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>
#include <noggit/types/NoggitTypes.h>
#include <noggit/database/repositories/NoggitLocaleStringRepository.h>

namespace Noggit::Database::Repositories
{
	struct AreaTableEntry
	{
		int Id;
		int ContinentId;
		int ParentAreaId;
		int AreaBit;
		int Flags;
		int SoundProviderPref;
		int SoundProviderPrefUnderwater;
		int AmbienceID;
		int ZoneMusic;
		int IntroSound;
		int ExplorationLevel;
		LocaleString AreaName;
		int FactionGroupMask;
		int LiquidTypeID[4];
		float MinElevation;
		float Ambient_multiplier;
		int LightID;
	};

	class IAreaTableRepository
	{

	};

	class WotlkAreaTableRepository : public IAreaTableRepository, public LocaleStringRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkAreaTableRepository(const std::filesystem::path& database_path) : LocaleStringRepository(database_path) ,_databasePath(database_path)
		{
		}

		std::vector<AreaTableEntry> GetAllAreasForMapId(unsigned int areaId)
		{

			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT ID, ContinentID, ParentAreaID, AreaBit, Flags, SoundProviderPref, SoundProviderPrefUnderwater, AmbienceID, ZoneMusic, IntroSound, "
				"ExplorationLevel, AreaName_lang, FactionGroupMask, LiquidTypeID_0, LiquidTypeID_1, LiquidTypeID_2, LiquidTypeID_3, MinElevation, Ambient_multiplier, LightID "
				"FROM AreaTable WHERE ContinentID = ?;");

			query.bind(1, areaId);

			auto areas = std::vector<AreaTableEntry>();
			while(query.executeStep())
			{
				auto area = AreaTableEntry();
				area.Id = query.getColumn(0).getInt();
				area.ContinentId = query.getColumn(1).getInt();
				area.ParentAreaId = query.getColumn(2).getInt();
				area.AreaBit = query.getColumn(3).getInt();
				area.Flags = query.getColumn(4).getInt();
				area.SoundProviderPref = query.getColumn(5).getInt();
				area.SoundProviderPrefUnderwater = query.getColumn(6).getInt();
				area.AmbienceID = query.getColumn(7).getInt();
				area.ZoneMusic = query.getColumn(8).getInt();
				area.IntroSound = query.getColumn(9).getInt();
				area.ExplorationLevel = query.getColumn(10).getInt();
				area.AreaName = GetLocaleString(query.getColumn(11).getInt());
				area.FactionGroupMask = query.getColumn(12).getInt();

				for(auto liquidType = 13; liquidType < 17; liquidType++)
				{
					area.LiquidTypeID[liquidType - 13] = query.getColumn(13).getInt();
				}

				area.MinElevation = query.getColumn(17).getDouble();
				area.Ambient_multiplier = query.getColumn(18).getDouble();
				area.LightID = query.getColumn(19).getInt();

				areas.push_back(area);
			}

			return areas;
		}

		uint32_t GetParentAreaId(unsigned int areaId)
		{
			if (!areaId || areaId == -1)
				return 0;

			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT ParentAreaID FROM AreaTable WHERE ID = ?; ");

			query.bind(1, areaId);

			if (query.executeStep())
			{
				return query.getColumn(0).getInt();
			}
		}

		LocaleString GetAreaName(unsigned int areaId)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT AreaName_lang FROM AreaTable WHERE ID = ?; ");

			query.bind(1, areaId);

			if (query.executeStep())
			{
				return GetLocaleString(query.getColumn(0).getInt());
			}
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