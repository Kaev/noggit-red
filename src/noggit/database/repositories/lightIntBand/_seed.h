#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	struct LightIntBandEntry
	{
		int Id;
		int Count;
		int Timings[16];
		int Data[16];
	};

	class ILightIntBandRepository
	{

	};

	class WotlkLightIntBandRepository : public ILightIntBandRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkLightIntBandRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}

		LightIntBandEntry GetLightIntBandEntryById(int id)
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT ID, Num, Time_0, Time_1, Time_2, Time_3, Time_4, Time_5, Time_6, Time_7, Time_8, Time_9, Time_10, Time_11, Time_12, Time_13, Time_14, Time_15, "
				"Data_0, Data_1, Data_2, Data_3, Data_4, Data_5, Data_6, Data_7, Data_8, Data_9, Data_10, Data_11, Data_12, Data_13, Data_14, Data_15 "
				"FROM LightIntBand WHERE ID = ?; ");

			query.bind(1, id);

			auto entry = LightIntBandEntry();

			if (query.executeStep())
			{
				entry.Id = query.getColumn(0).getInt();
				entry.Count = query.getColumn(1).getInt();

				for (auto timeIndex = 2; timeIndex < 18; timeIndex++)
				{
					entry.Timings[timeIndex - 2] = query.getColumn(timeIndex).getInt();
				}

				for (auto dataIndex = 18; dataIndex < 34; dataIndex++)
				{
					entry.Data[dataIndex - 18] = query.getColumn(dataIndex).getInt();
				}
			}

			return entry;
		}
	};

	class ShadowlandsLightIntBandRepository : public ILightIntBandRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsLightIntBandRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}
	};
}