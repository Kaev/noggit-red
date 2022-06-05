// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/DBC.h>
#include <noggit/Log.h>
#include <noggit/Misc.h>
#include <blizzard-archive-library/include/ClientData.hpp>
#include <string>

LightDB gLightDB;
LightParamsDB gLightParamsDB;
LightSkyboxDB gLightSkyboxDB;
LightIntBandDB gLightIntBandDB;
LightFloatBandDB gLightFloatBandDB;
LiquidTypeDB gLiquidTypeDB;

void OpenDBs(std::shared_ptr<BlizzardArchive::ClientData> clientData)
{
  gLightDB.open(clientData);
  gLightParamsDB.open(clientData);
  gLightSkyboxDB.open(clientData);
  gLightIntBandDB.open(clientData);
  gLightFloatBandDB.open(clientData);
  gLiquidTypeDB.open(clientData);
}

int LiquidTypeDB::getLiquidType(int pID)
{
  int type = 0;
  try
  {
    LiquidTypeDB::Record rec = gLiquidTypeDB.getByID(pID);
    type = rec.getUInt(LiquidTypeDB::Type);
  }
  catch (LiquidTypeDB::NotFound)
  {
    type = 0;
  }
  return type;
}

std::string  LiquidTypeDB::getLiquidName(int pID)
{
  std::string type = "";
  try
  {
    LiquidTypeDB::Record rec = gLiquidTypeDB.getByID(pID);
    type = std::string(rec.getString(LiquidTypeDB::Name));
  }
  catch (LiquidTypeDB::NotFound)
  {
    type = "Unknown type";
  }

  return type;
}
