#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	struct LightParamEntry
	{
		int Id;
		int HighlightSky;
		int LightSkyboxId;
		float Glow;
		float WaterShallowAlpha;
		float WaterDeepAlpha;
		float OceanShallowAlpha;
		float OceanDeepAlpha;
		int Flags;
	};

	class ILightParamsRepository
	{

	};

	class WotlkLightParamsRepository : public ILightParamsRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLightParamsRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}

		LightParamEntry GetLightsForMapId(int lightParamId)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT ID, HighlightSky, LightSkyboxID, Glow, WaterShallowAlpha, WaterDeepAlpha, OceanShallowAlpha, OceanDeepAlpha, Flags "
				"FROM LightParams WHERE ID = ? ;");

			query.bind(1, lightParamId);

			if(query.executeStep())
			{
				auto lightParamEntry = LightParamEntry();

				lightParamEntry.Id = query.getColumn(0).getInt();
				lightParamEntry.HighlightSky = query.getColumn(1).getInt();
				lightParamEntry.LightSkyboxId = query.getColumn(2).getInt();

				lightParamEntry.Glow = query.getColumn(3).getDouble();
				lightParamEntry.WaterShallowAlpha = query.getColumn(4).getDouble();
				lightParamEntry.WaterDeepAlpha = query.getColumn(5).getDouble();
				lightParamEntry.OceanShallowAlpha = query.getColumn(6).getDouble();
				lightParamEntry.OceanDeepAlpha = query.getColumn(7).getDouble();
#
				lightParamEntry.Flags = query.getColumn(8).getInt();

				return lightParamEntry;
			}
		}
	};

	class ShadowlandsLightParamsRepository : public ILightParamsRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLightParamsRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}