#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	class IGroundEffectTextureRepository
	{

	};

	class WotlkGroundEffectTextureRepository : public IGroundEffectTextureRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkGroundEffectTextureRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

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