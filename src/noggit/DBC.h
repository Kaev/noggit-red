// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/DBCFile.h>

#include <string>

class AreaDB : public DBCFile
{
public:
  AreaDB() :
    DBCFile("DBFilesClient\\AreaTable.dbc")
  { }

  /// Fields
  static const size_t AreaID = 0;    // uint
  static const size_t Continent = 1;  // uint
  static const size_t Region = 2;    // uint [AreaID]
  static const size_t Flags = 4;    // bit field
  static const size_t Name = 11;    // localisation string

  static std::string getAreaName(int pAreaID);
  static std::uint32_t get_area_parent(int area_id);
};

class LightDB : public DBCFile
{
public:
  LightDB() :
    DBCFile("DBFilesClient\\Light.dbc")
  { }

  /// Fields
  static const size_t ID = 0;        // uint
  static const size_t Map = 1;      // uint
  static const size_t PositionX = 2;    // float
  static const size_t PositionY = 3;    // float
  static const size_t PositionZ = 4;    // float
  static const size_t RadiusInner = 5;  // float
  static const size_t RadiusOuter = 6;  // float
  static const size_t DataIDs = 7;    // uint[8]
};

class LightParamsDB : public DBCFile{
public:
  LightParamsDB() :
    DBCFile("DBFilesClient\\LightParams.dbc")
  { }

  /// Fields
  static const size_t ID = 0;        // uint
  static const size_t highlightSky = 1;// bool
  static const size_t skybox = 2;      // uint ref to LightSkyBox
  static const size_t cloudTypeID = 3; // uint
  static const size_t glow = 4;        // uint
  static const size_t water_shallow_alpha = 5;
  static const size_t water_deep_alpha = 6;
  static const size_t ocean_shallow_alpha = 7;
  static const size_t ocean_deep_alpha = 8;
  static const size_t flags = 9;
};

class LightSkyboxDB : public DBCFile
{
public:
  LightSkyboxDB() :
    DBCFile("DBFilesClient\\LightSkybox.dbc")
  { }

  /// Fields
  static const size_t ID = 0;        // uint
  static const size_t filename = 1;    // string
  static const size_t flags = 2;      // uint
};

class LightIntBandDB : public DBCFile
{
public:
  LightIntBandDB() :
    DBCFile("DBFilesClient\\LightIntBand.dbc")
  { }

  /// Fields
  static const size_t ID = 0;        // uint
  static const size_t Entries = 1;    // uint
  static const size_t Times = 2;      // uint
  static const size_t Values = 18;    // uint
};

class LightFloatBandDB : public DBCFile
{
public:
  LightFloatBandDB() :
    DBCFile("DBFilesClient\\LightFloatBand.dbc")
  { }

  /// Fields
  static const size_t ID = 0;        // uint
  static const size_t Entries = 1;    // uint
  static const size_t Times = 2;      // uint
  static const size_t Values = 18;    // float
};

class LiquidTypeDB : public DBCFile
{
public:
  LiquidTypeDB() :
    DBCFile("DBFilesClient\\LiquidType.dbc")
  { }

  /// Fields
  static const size_t ID = 0;        // uint
  static const size_t Name = 1;      // string
  static const size_t Type = 3;      // uint
  static const size_t ShaderType = 14;  // uint
  static const size_t TextureFilenames = 15;    // string[6]
  static const size_t TextureTilesPerBlock = 23;  // uint
  static const size_t Rotation = 24;  // uint
  static const size_t AnimationX = 23;  // uint
  static const size_t AnimationY = 24;  // uint

  static int getLiquidType(int pID);
  static std::string getLiquidName(int pID);
};

void OpenDBs(std::shared_ptr<BlizzardArchive::ClientData> clientData);

extern AreaDB gAreaDB;
extern LightDB gLightDB;
extern LightParamsDB gLightParamsDB;
extern LightSkyboxDB gLightSkyboxDB;
extern LightIntBandDB gLightIntBandDB;
extern LightFloatBandDB gLightFloatBandDB;
extern LiquidTypeDB gLiquidTypeDB;
