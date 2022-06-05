#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	struct DoodadEffectTextureEntry
	{
		int Id;
		int DoodadIds[4];
		int DoodadWeights[4];
		int Density;
		int Sound;

	};

	class IGroundEffectTextureRepository
	{
		virtual DoodadEffectTextureEntry GetDoodadEffectTextureById(int doodadId) = 0;
	};

	class WotlkGroundEffectTextureRepository : public IGroundEffectTextureRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkGroundEffectTextureRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}

		DoodadEffectTextureEntry GetDoodadEffectTextureById(int doodadId) override
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT ID, DoodadID_0, DoodadID_1, DoodadID_2, DoodadID_3,"
										 " DoodadWeight_0, DoodadWeight_1, DoodadWeight_2, DoodadWeight_3,"
										 " Density, Sound FROM GroundEffectTexture WHERE ID = ?;");

			query.bind(1, doodadId);

			if (query.executeStep())
			{
				auto entry = DoodadEffectTextureEntry();
				entry.Id = query.getColumn(0).getInt();

				for (int doodadIdIndex = 1; doodadIdIndex < 5; doodadIdIndex++)
				{
					entry.DoodadIds[doodadIdIndex - 1] = query.getColumn(doodadIdIndex).getInt();
				}

				for (int doodadWeightIndex = 5; doodadWeightIndex < 9; doodadWeightIndex++)
				{
					entry.DoodadWeights[doodadWeightIndex -5] = query.getColumn(doodadWeightIndex).getInt();
				}

				entry.Density = query.getColumn(9).getInt();
				entry.Sound = query.getColumn(10).getInt();

				return entry;
			}
		}
	};

	class ShadowlandsGroundEffectTextureRepository : public IGroundEffectTextureRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsGroundEffectTextureRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}