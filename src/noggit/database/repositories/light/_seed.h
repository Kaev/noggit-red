#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>

namespace Noggit::Database::Repositories
{
	struct Position
	{
		float X;
		float Y;
		float Z;
	};

	struct LightEntry
	{
		int Id;
		int ContinentId;
		Position Position;
		float GameFalloffStart;
		float GameFalloffEnd;
		int LightParamsId[8];
	};

	class ILightRepository
	{

	};

	class WotlkLightRepository : public ILightRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLightRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}

		std::vector<LightEntry> GetLightsForMapId(int mapId)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT ID, ContinentID, GameCoords_0, GameCoords_1, GameCoords_2, GameFalloffStart, GameFalloffEnd, "
				"LightParamsID_0, LightParamsID_1, LightParamsID_2, LightParamsID_3, LightParamsID_4, LightParamsID_5, LightParamsID_6, LightParamsID_7 "
				"FROM Light WHERE ContinentID = ?;");

			query.bind(1, mapId);

			auto listOfLight = std::vector<LightEntry>();
			while(query.executeStep())
			{
				auto lightEntry = LightEntry();
				lightEntry.Id = query.getColumn(0).getInt();
				lightEntry.ContinentId = query.getColumn(1).getInt();

				lightEntry.Position = Position();
				lightEntry.Position.X = query.getColumn(2).getDouble();
				lightEntry.Position.Y = query.getColumn(3).getDouble();
				lightEntry.Position.Z = query.getColumn(4).getDouble();

				lightEntry.GameFalloffStart = query.getColumn(5).getDouble();
				lightEntry.GameFalloffEnd = query.getColumn(6).getDouble();

				for(int lightParamsIndex = 7; lightParamsIndex < 15; lightParamsIndex++)
				{
					lightEntry.LightParamsId[lightParamsIndex - 7] = query.getColumn(lightParamsIndex).getDouble();
				}

				listOfLight.push_back(lightEntry);
			}

			return listOfLight;
		}
	};

	class ShadowlandsLightRepository : public ILightRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLightRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}