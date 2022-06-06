// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/DBC.h>
#include <noggit/Log.h>
#include <noggit/Misc.h>
#include <blizzard-archive-library/include/ClientData.hpp>
#include <string>

LightParamsDB gLightParamsDB;


void OpenDBs(std::shared_ptr<BlizzardArchive::ClientData> clientData)
{
  gLightParamsDB.open(clientData);
}
