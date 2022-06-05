#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/types/NoggitTypes.h>
#include <filesystem>

namespace Noggit::Database
{ 
 class LocaleStringRepository
    {
        std::filesystem::path _databasePath;
    public:
        LocaleStringRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
        {

        }

        LocaleString GetLocaleString(int id)
        {
            SQLite::Database   db(_databasePath.generic_string());
            SQLite::Statement  selectLocaleStringQuery(db, "SELECT enUS, koKR, frFR, deDE, zhCN, zhTW, esES, esMX, ruRU,"
                "jaJP, ptPT, itIT, Unk1, Unk2, Unk3,Unk4, flags "
                "FROM _localeStrings WHERE id = ?");

            selectLocaleStringQuery.bind(1, id);

            if (selectLocaleStringQuery.executeStep())
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

        int CreateLocaleString(LocaleString string)
        {
            SQLite::Database   db(_databasePath.generic_string(), SQLite::OPEN_READWRITE);
            SQLite::Statement  updateLocaleString(db, "INSERT INTO _localeStrings(enUS, koKR, frFR, deDE, zhCN, zhTW, esES, esMX, ruRU, jaJP, ptPT, itIT, Unk1, Unk2, Unk3, Unk4, flags)"
                "VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? )");

            updateLocaleString.bind(1, string[Locale::enUS]);
            updateLocaleString.bind(2, string[Locale::koKR]);
            updateLocaleString.bind(3, string[Locale::frFR]);
            updateLocaleString.bind(4, string[Locale::deDE]);
            updateLocaleString.bind(5, string[Locale::zhCN]);
            updateLocaleString.bind(6, string[Locale::zhTW]);
            updateLocaleString.bind(7, string[Locale::esES]);
            updateLocaleString.bind(8, string[Locale::esMX]);
            updateLocaleString.bind(9, string[Locale::ruRU]);
            updateLocaleString.bind(10, string[Locale::jaJP]);
            updateLocaleString.bind(11, string[Locale::ptPT]);
            updateLocaleString.bind(12, string[Locale::itIT]);
            updateLocaleString.bind(13, string[Locale::Unk1]);
            updateLocaleString.bind(14, string[Locale::Unk2]);
            updateLocaleString.bind(15, string[Locale::Unk3]);
            updateLocaleString.bind(16, string[Locale::Unk4]);
            updateLocaleString.bind(17, string.flags);
            updateLocaleString.exec();

            SQLite::Statement  getLastId(db, "SELECT last_insert_rowid()");
            if (getLastId.executeStep())
            {
                string.id = getLastId.getColumn(0).getInt();
                return string.id;
            }

            auto errorMessage = std::string(db.getErrorMsg());
        }

        void UpdateLocaleString(LocaleString string)
        {
            SQLite::Database   db(_databasePath.generic_string(), SQLite::OPEN_READWRITE);
            SQLite::Statement  updateLocaleString(db, "UPDATE _localeStrings SET enUS = ? , koKR = ? , frFR = ? , deDE = ? ,"
                "zhCN = ? , zhTW = ? , esES = ? , esMX = ? , ruRU = ? , jaJP = ? , "
                "ptPT = ? , itIT = ? , Unk1 = ? , Unk2 = ? , Unk3 = ? , Unk4 = ? , flags = ? "
                "WHERE id = ?");

            updateLocaleString.bind(1, string[Locale::enUS]);
            updateLocaleString.bind(2, string[Locale::koKR]);
            updateLocaleString.bind(3, string[Locale::frFR]);
            updateLocaleString.bind(4, string[Locale::deDE]);
            updateLocaleString.bind(5, string[Locale::zhCN]);
            updateLocaleString.bind(6, string[Locale::zhTW]);
            updateLocaleString.bind(7, string[Locale::esES]);
            updateLocaleString.bind(8, string[Locale::esMX]);
            updateLocaleString.bind(9, string[Locale::ruRU]);
            updateLocaleString.bind(10, string[Locale::jaJP]);
            updateLocaleString.bind(11, string[Locale::ptPT]);
            updateLocaleString.bind(12, string[Locale::itIT]);
            updateLocaleString.bind(13, string[Locale::Unk1]);
            updateLocaleString.bind(14, string[Locale::Unk2]);
            updateLocaleString.bind(15, string[Locale::Unk3]);
            updateLocaleString.bind(16, string[Locale::Unk4]);
            updateLocaleString.bind(17, string.flags);
            updateLocaleString.bind(18, string.id);

            updateLocaleString.exec();

            auto errorMessage = std::string(db.getErrorMsg());
        }

};
}