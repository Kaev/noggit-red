#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	struct LiquidTypeEntry
	{
		int Id;
		std::string Name;
		int Flags; //Liquidype?
		int SoundBank;
		int SoundId;
		int SpellId;
		float MaxDarkenDepth;
		float FogDarkenIntensity;
		float AmbDarkenIntensity;
		float DirDarkenIntensity;
		int LightId;
		float ParticleScale;
		int ParticleMovement;
		int ParticleTexSlots;
		int MaterialID;
		std::string Textures[5];
		int Color[2];
		float UnknownFloats_18[18];
		int UnknownInts_4[4];


		//Custom
		float Animation_x;
		float Animation_y;
	};

	class ILiquidTypeRepository
	{

	};

	class WotlkLiquidTypeRepository : public ILiquidTypeRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLiquidTypeRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}

		std::vector<LiquidTypeEntry> GetLiquidTypes()
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT ID, Name, Flags, SoundBank, SoundID, SpellID, MaxDarkenDepth, FogDarkenIntensity, AmbDarkenIntensity, DirDarkenIntensity, "
				"LightID, ParticleScale, ParticleMovement, ParticleTexSlots, MaterialID, Texture_0, Texture_1, Texture_2, Texture_3, Texture_4, Texture_5, Color_0, Color_1, "
				"Float_0, Float_1, Float_2, Float_3, Float_4, Float_5, Float_6, Float_7, Float_8, Float_9, Float_10, Float_11, Float_12, Float_13, Float_14, Float_15, Float_16, Float_17, "
				"Int_0, Int_1, Int_2, Int_3 "
				"FROM LiquidType;");

			auto liquidTypes = std::vector<LiquidTypeEntry>();
			while(query.executeStep())
			{
				auto entry = LiquidTypeEntry();
				entry.Id = query.getColumn(0).getInt();
				entry.Name = query.getColumn(1).getString();
				entry.Flags = query.getColumn(2).getInt();
				entry.SoundBank = query.getColumn(3).getInt();
				entry.SoundId = query.getColumn(4).getInt();
				entry.SpellId = query.getColumn(5).getInt();
				entry.MaxDarkenDepth = query.getColumn(6).getDouble();
				entry.FogDarkenIntensity = query.getColumn(7).getDouble();
				entry.AmbDarkenIntensity = query.getColumn(8).getDouble();
				entry.DirDarkenIntensity = query.getColumn(9).getDouble();
				entry.LightId = query.getColumn(10).getInt();
				entry.ParticleScale = query.getColumn(11).getDouble();
				entry.ParticleMovement = query.getColumn(12).getInt();
				entry.ParticleTexSlots = query.getColumn(13).getInt();
				entry.MaterialID = query.getColumn(14).getInt();

				for(auto textureIndex = 15; textureIndex< 19; textureIndex++)
				{
					entry.Textures[textureIndex - 15] = query.getColumn(textureIndex).getString();
				}

				for (auto colorIndex = 19; colorIndex < 21; colorIndex++)
				{
					entry.Color[colorIndex - 19] = query.getColumn(colorIndex).getInt();
				}

				for (auto unknownFloats = 21; unknownFloats < 39; unknownFloats++)
				{
					entry.UnknownFloats_18[unknownFloats - 21] = query.getColumn(unknownFloats).getDouble();
				}
	
				entry.Animation_x = entry.UnknownFloats_18[2];
				entry.Animation_y = entry.UnknownFloats_18[3];

				for (auto unknownInts = 39; unknownInts < 43; unknownInts++)
				{
					entry.UnknownInts_4[unknownInts - 39] = query.getColumn(unknownInts).getInt();
				}

				liquidTypes.push_back(entry);
			}

			return liquidTypes;
		}

		int GetLiquidTypeByLiquidId(int liquidId)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT SoundBank FROM LiquidType WHERE ID = ?; "); //SoundBank is what Dbd says? is that wrong?

			query.bind(1, liquidId);

			if (query.executeStep())
			{
				return query.getColumn(0).getInt();
			}
		}

		std::string GetLiquidNameByLiquidId(int liquidId)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT Name FROM LiquidType WHERE ID = ?; ");

			query.bind(1, liquidId);

			if (query.executeStep())
			{
				return query.getColumn(0).getString();
			}
		}
	};

	class ShadowlandsLiquidTypeRepository : public ILiquidTypeRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLiquidTypeRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}