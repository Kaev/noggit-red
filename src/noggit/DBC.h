// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/DBCFile.h>
#include <string>

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

void OpenDBs(std::shared_ptr<BlizzardArchive::ClientData> clientData);

extern LightDB gLightDB;
extern LightParamsDB gLightParamsDB;
extern LightIntBandDB gLightIntBandDB;
extern LightFloatBandDB gLightFloatBandDB;
