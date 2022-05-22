#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database
{
    ApplicationProjectDatabase::ApplicationProjectDatabase(std::shared_ptr<Application::NoggitApplicationConfiguration> configuration, std::vector<std::string> tables) 
        : _configuration(configuration), _tables(tables)
    {
    
    }

    void ApplicationProjectDatabase::CreateDatabase(std::filesystem::path const& database_path, std::filesystem::path const& project_path,
        std::filesystem::path const& client_path, ProjectBuildInformation& buildInformation)
    {

        BlizzardDatabaseLib::Structures::Build client_build("3.3.5.12340");
        auto client_archive_version = BlizzardArchive::ClientVersion::WOTLK;
        auto client_archive_locale = BlizzardArchive::Locale::AUTO;
        if (buildInformation.Version == ProjectVersion::SL)
        {
            client_archive_version = BlizzardArchive::ClientVersion::SL;
            client_build = BlizzardDatabaseLib::Structures::Build("9.1.0.39584");
            client_archive_locale = BlizzardArchive::Locale::enUS;
        }

        std::string dbd_file_directory = _configuration->ApplicationDatabaseDefinitionsPath;
        auto clientDatabase = BlizzardDatabaseLib::BlizzardDatabase(dbd_file_directory, client_build);

        auto clientData = BlizzardArchive::ClientData(client_path.generic_string(),
            client_archive_version, client_archive_locale, project_path.generic_string());

        auto databseAlreadyExists = std::filesystem::exists(database_path);
        auto database = SQLite::Database(database_path.generic_string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        SQLite::Transaction transaction(database);

        if (!databseAlreadyExists)
        {
            database.exec("CREATE TABLE _version (expansion INTEGER, major INTEGER, minor INTEGER, patch INTEGER)");

            auto createVersionTableQuery = SQLite::Statement(database, "INSERT INTO _version(expansion,major,minor,patch) VALUES(?,?,?,?)");

            createVersionTableQuery.bind(1, buildInformation.Expansion);
            createVersionTableQuery.bind(2, buildInformation.Major);
            createVersionTableQuery.bind(3, buildInformation.Minor);
            createVersionTableQuery.bind(4, buildInformation.Patch);
            createVersionTableQuery.exec();

            database.exec("CREATE TABLE _localeStrings (id INTEGER PRIMARY KEY AUTOINCREMENT,enUS TEXT,koKR TEXT,frFR TEXT,deDE TEXT,zhCN TEXT,"
                                                                                            "zhTW TEXT,esES TEXT,esMX TEXT,ruRU TEXT,jaJP TEXT,"
                                                                                            "ptPT TEXT,itIT TEXT,Unk1 TEXT,Unk2 TEXT,Unk3 TEXT, Unk4 TEXT,"
                                                                                            "flags INTEGER)");

            database.exec("CREATE TABLE _schema (id INTEGER PRIMARY KEY AUTOINCREMENT, table_name TEXT, column_name TEXT, column_type TEXT, column_flags INTEGER, column_array_length INTEGER)");
            database.exec("CREATE TABLE _tables (table_name TEXT)");
        }

        for (auto const& tableName : _tables)
        {
            auto createTableQuery = SQLite::Statement(database, "INSERT INTO _tables(table_name) VALUES(?)");
            createTableQuery.bind(1, tableName);
            createTableQuery.exec();

            auto databaseTable = clientDatabase.LoadTable(tableName,
                [&](std::string const& file_path) -> std::shared_ptr<BlizzardDatabaseLib::Stream::IMemStream>
                {
                    BlizzardArchive::ClientFile f(file_path, &clientData);
                    return std::make_shared<BlizzardDatabaseLib::Stream::IMemStream>(f.getBuffer(), f.getSize());
                });

            auto tableCreationColumnStream = std::stringstream();
            auto tableCreationForeignKeyStream = std::stringstream();
            auto tableDefinitions = databaseTable.GetRecordDefinition();
            tableCreationColumnStream << "CREATE TABLE " << tableName << "(";
            for (auto const& def : tableDefinitions)
            {
                auto columnName = def.Name;
                auto columnIsArray = def.arrLength > 0;
                auto type = MapDefinitionTypeToDatabaseType(def.Type);

                TableColumnFlags flags = TableColumnFlags::None;
                if (def.isID)      flags = flags | TableColumnFlags::IsId;
                if (columnIsArray) flags = flags | TableColumnFlags::IsArray;

                auto columnSchemaInsertQuery = SQLite::Statement(database, "INSERT INTO _schema(table_name,column_name,column_type,column_flags,column_array_length) VALUES(?,?,?,?,?)");
                columnSchemaInsertQuery.bind(1, tableName);
                columnSchemaInsertQuery.bind(2, def.Name);
                columnSchemaInsertQuery.bind(3, def.Type);
                columnSchemaInsertQuery.bind(4, static_cast<int>(flags));
                columnSchemaInsertQuery.bind(5, def.arrLength);
                columnSchemaInsertQuery.exec();

                if (columnIsArray)
                {
                    for (int i = 0; i < def.arrLength; i++)
                    {
                        tableCreationColumnStream << columnName << "_" << std::to_string(i) << " " << type << ",";
                    }
                }
                else
                {
                    if (def.isID)
                        tableCreationColumnStream << columnName << " " << type << " PRIMARY KEY,";
                    if (!def.isID)
                        tableCreationColumnStream << columnName << " " << type << ",";
                    if (def.Type == "locstring")
                        tableCreationForeignKeyStream << "FOREIGN KEY(" << columnName << ") REFERENCES _localeStrings(id),";
                }
            }

            tableCreationForeignKeyStream.seekp(-1, tableCreationForeignKeyStream.cur) << " ";
            if (tableCreationForeignKeyStream.str().size() == 0)
            {
                tableCreationColumnStream.seekp(-1, tableCreationColumnStream.cur) << " ";
            }

            auto tableCreationStream = std::stringstream();
            tableCreationStream << tableCreationColumnStream.str() << tableCreationForeignKeyStream.str() << ");";
            database.exec(tableCreationStream.str());

            auto iterator = databaseTable.Records();
            while (iterator.HasRecords())
            {
                auto recordCreationStream = std::stringstream();
                auto valueStream = std::stringstream();
                auto columnStream = std::stringstream();
                auto record = iterator.Next();

                recordCreationStream << "INSERT INTO " << tableName;
                columnStream << "(";
                valueStream << "VALUES(";

                for (auto const& def : tableDefinitions)
                {
                    auto recordValue = record.Columns[def.Name];
                    if (def.Name == "ID")
                        recordValue.Value = std::to_string(record.RecordId);

                    if (def.arrLength > 0)
                    {
                        for (int i = 0; i < def.arrLength; i++)
                        {
                            if (def.Type == "string")
                            {
                                columnStream << def.Name << "_" << std::to_string(i) << ",";
                                std::replace(recordValue.Values[i].begin(), recordValue.Values[i].end(), '\"', '\'');
                                valueStream << "\"" << recordValue.Values[i] << "\"" << ",";
                            }
                            else
                            {
                                columnStream << def.Name << "_" << std::to_string(i) << ",";
                                valueStream << recordValue.Values[i] << ",";
                            }
                        }
                    }
                    else
                    {
                        if (def.Type == "locstring")
                        {
                            auto locstringInsertStream = std::stringstream();
                            auto locstringvalueStream = std::stringstream();
                            auto locstringcolumnStream = std::stringstream();

                            locstringInsertStream << "INSERT INTO _localeStrings";
                            locstringcolumnStream << "(";
                            locstringvalueStream << "VALUES(";

                            for (int localeIndex = 0; localeIndex < 16; localeIndex++)
                            {
                                auto locale = LocaleNames[localeIndex];
                                auto localeValue = recordValue.Values[localeIndex];

                                std::replace(localeValue.begin(), localeValue.end(), '\"', '\'');

                                locstringcolumnStream << locale << ",";
                                locstringvalueStream << "\"" << localeValue << "\",";
                            }

                            auto flagsName = def.Name + "_flags";
                            locstringvalueStream << record.Columns[flagsName].Value << ")";
                            locstringcolumnStream << "flags" << ")";

                            locstringInsertStream << locstringcolumnStream.str() << locstringvalueStream.str();

                            database.exec(locstringInsertStream.str());

                            auto getLastRowId = "SELECT last_insert_rowid()";
                            auto lastInsertId = database.execAndGet(getLastRowId).getInt();

                            columnStream << def.Name << ",";
                            valueStream << lastInsertId << ",";
                        }
                        else if (def.Type == "string")
                        {
                            columnStream << def.Name << ",";
                            std::replace(recordValue.Value.begin(), recordValue.Value.end(), '\"', '\'');
                            valueStream << "\"" << recordValue.Value << "\"" << ",";
                        }
                        else
                        {
                            columnStream << def.Name << ",";
                            valueStream << recordValue.Value << ",";
                        }
                    }
                }
                columnStream.seekp(-1, columnStream.cur);
                valueStream.seekp(-1, valueStream.cur);
                valueStream << ")";
                columnStream << ")";

                recordCreationStream << columnStream.str() << " " << valueStream.str();
                database.exec(recordCreationStream.str());
            }

            clientDatabase.UnloadTable(tableName);
        }

        transaction.commit();
    }

    void ApplicationProjectDatabase::ExportDatabase(std::filesystem::path const& database_path, std::filesystem::path const& project_path,
        std::filesystem::path const& client_path, ProjectBuildInformation& buildInformation)
    {
        BlizzardDatabaseLib::Structures::Build client_build("3.3.5.12340");
        auto client_archive_version = BlizzardArchive::ClientVersion::WOTLK;
        auto client_archive_locale = BlizzardArchive::Locale::AUTO;
        if (buildInformation.Version == ProjectVersion::SL)
        {
            client_archive_version = BlizzardArchive::ClientVersion::SL;
            client_build = BlizzardDatabaseLib::Structures::Build("9.1.0.39584");
            client_archive_locale = BlizzardArchive::Locale::enUS;
        }

        auto database = SQLite::Database(database_path.generic_string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        SQLite::Statement  tablesQuery(database, "SELECT table_name FROM _tables");

        auto tableList = std::vector<std::string>();
        while (tablesQuery.executeStep())
        {
            std::string tableName = tablesQuery.getColumn(0).getString();
            tableList.push_back(tableName);
        }

        for (auto const& table : tableList)
        {
            SQLite::Statement  schemaQuery(database, "SELECT id, column_name, column_type, column_flags, column_array_length FROM _schema where table_name = ?");
            schemaQuery.bind(1, table);

            auto tableSchema = std::map<int, DatabaseColumnSchema>();
            auto counter = 0;
            while (schemaQuery.executeStep())
            {
                auto tableColumnSchema = DatabaseColumnSchema();
                tableColumnSchema.ColumnId = schemaQuery.getColumn(0).getInt();
                tableColumnSchema.ColumnName = schemaQuery.getColumn(1).getString();
                tableColumnSchema.ColumnType = schemaQuery.getColumn(2).getString();
                tableColumnSchema.ColumnFlags = static_cast<TableColumnFlags>(schemaQuery.getColumn(3).getInt());
                tableColumnSchema.ColumnArrayLength = schemaQuery.getColumn(4).getInt();

                tableSchema.emplace(counter, tableColumnSchema);
                counter++;
            }

            auto selectTableContentsQueryStream = std::stringstream();
            selectTableContentsQueryStream << "SELECT ";
            for (auto const& columnSchema : tableSchema)
            {
                auto columnName = columnSchema.second.ColumnName;
                auto columnDef = columnSchema.second;

                if (columnDef.ColumnFlags & TableColumnFlags::IsArray)
                {
                    for (auto index = 0; index < columnDef.ColumnArrayLength; index++)
                    {
                        selectTableContentsQueryStream << columnName << "_" << std::to_string(index) << ",";
                    }
                }
                else
                {
                    selectTableContentsQueryStream << columnName << ",";
                }
            }

            selectTableContentsQueryStream.seekp(-1, selectTableContentsQueryStream.cur);
            selectTableContentsQueryStream << " FROM " << table;

            SQLite::Statement  selectDataQuery(database, selectTableContentsQueryStream.str());
            auto databaseRowList = std::vector<BlizzardDatabaseLib::Structures::BlizzardDatabaseRow>();
            auto schemaLength = tableSchema.size();
            while (selectDataQuery.executeStep())
            {
                auto databaseRow = BlizzardDatabaseLib::Structures::BlizzardDatabaseRow();
                auto offset = 0;
                for (int index = 0; index < schemaLength; index++)
                {
                    auto databaseColumn = BlizzardDatabaseLib::Structures::BlizzardDatabaseColumn();
                    auto column = selectDataQuery.getColumn(index + offset);
                    auto columnDefintion = tableSchema[index];

                    if (columnDefintion.ColumnFlags & TableColumnFlags::IsId)
                    {
                        databaseRow.RecordId = column.getInt();
                        databaseColumn.Value = std::to_string(databaseRow.RecordId);
                    }
                    else if (columnDefintion.ColumnFlags & TableColumnFlags::IsArray)
                    {
                        auto arrayLength = columnDefintion.ColumnArrayLength;
                        databaseColumn.Values = std::vector<std::string>();
                        for (auto arrayLengthIndex = 0; arrayLengthIndex < arrayLength; arrayLengthIndex++)
                        {
                            auto arrayIndexedColumn = selectDataQuery.getColumn(index + offset + arrayLengthIndex);
                            databaseColumn.Values.push_back(arrayIndexedColumn.getString());
                        }
                        offset += arrayLength - 1;
                    }
                    else if (columnDefintion.ColumnType == "locstring")
                    {
                        auto columnId = column.getInt();

                        SQLite::Statement  selectLocaleStringQuery(database, "SELECT enUS, koKR, frFR, deDE, zhCN, zhTW, esES, esMX, ruRU,"
                            "jaJP, ptPT, itIT, Unk1, Unk2, Unk3,Unk4,flags"
                            " FROM _localeStrings WHERE id = ?");

                        auto errorMessage = std::string(database.getErrorMsg());
                        selectLocaleStringQuery.bind(1, columnId);
                        selectLocaleStringQuery.executeStep();

                        databaseColumn.Values = std::vector<std::string>();
                        for (int i = 0; i < 16; i++)
                        {
                            auto localeValue = selectLocaleStringQuery.getColumn(i).getString();
                            databaseColumn.Values.push_back(localeValue);
                        }
                        auto flagsLocaleStringColumn = BlizzardDatabaseLib::Structures::BlizzardDatabaseColumn();
                        flagsLocaleStringColumn.Value = selectLocaleStringQuery.getColumn(16).getString();
                        databaseRow.Columns.emplace(columnDefintion.ColumnName + "_flags", flagsLocaleStringColumn);
                    }
                    else
                    {
                        databaseColumn.Value = column.getString();
                    }

                    databaseRow.Columns.emplace(columnDefintion.ColumnName, databaseColumn);
                }

                databaseRowList.push_back(databaseRow);
            }

            std::string dbd_file_directory = _configuration->ApplicationDatabaseDefinitionsPath;
            auto clientDatabase = BlizzardDatabaseLib::BlizzardDatabase(dbd_file_directory, client_build);

            clientDatabase.SaveTable(project_path.generic_string(), table, databaseRowList);
        }
    }
}
