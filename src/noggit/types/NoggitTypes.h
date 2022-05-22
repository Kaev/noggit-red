#pragma once

namespace Noggit
{
  enum class ProjectVersion
  {
    VANILLA,
    BC,
    WOTLK,
    CATA,
    PANDARIA,
    WOD,
    LEGION,
    BFA,
    SL
  };

  struct ProjectBuildInformation
  {
      ProjectVersion Version;
      int Expansion;
      int Major;
      int Minor;
      int Patch;
  };

  static std::vector<std::string> Tables = {
              std::string("Map"),
              std::string("AreaTable"),
              std::string("LoadingScreens"),
              std::string("Light"),
              std::string("LightParams"),
              std::string("LightSkybox"),
              std::string("LightFloatBand"),
              std::string("GroundEffectDoodad"),
              std::string("GroundEffectTexture"),
              std::string("LiquidType")
  };

  static struct Locale
  {
      static constexpr const char* enUS = "enUS";
      static constexpr const char* koKR = "koKR";
      static constexpr const char* frFR = "frFR";
      static constexpr const char* deDE = "deDE";
      static constexpr const char* zhCN = "zhCN";
      static constexpr const char* zhTW = "zhTW";
      static constexpr const char* esES = "esES";
      static constexpr const char* esMX = "esMX";
      static constexpr const char* ruRU = "ruRU";
      static constexpr const char* jaJP = "jaJP";
      static constexpr const char* ptPT = "ptPT";
      static constexpr const char* itIT = "itIT";
      static constexpr const char* Unk1 = "Unk1";
      static constexpr const char* Unk2 = "Unk2";
      static constexpr const char* Unk3 = "Unk3";
      static constexpr const char* Unk4 = "Unk4";
  };

  static std::vector<std::string> LocaleNames = {
          Locale::enUS,
          Locale::koKR,
          Locale::frFR,
          Locale::deDE,
          Locale::zhCN,
          Locale::zhTW,
          Locale::esES,
          Locale::esMX,
          Locale::ruRU,
          Locale::jaJP,
          Locale::ptPT,
          Locale::itIT,
          Locale::Unk1,
          Locale::Unk2,
          Locale::Unk3,
          Locale::Unk4
  };

  struct LocaleString
  {
      int id;
      std::map<std::string, std::string> values;
      uint32_t flags;

      const std::string& operator[] (const std::string& locale)
      {
          return values[locale];
      }

      const std::string& operator[] (const char* locale)
      {
          return values[std::string(locale)];
      }
  };

}
