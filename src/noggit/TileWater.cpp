// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ChunkWater.hpp>
#include <noggit/Log.h>
#include <noggit/MapTile.h>
#include <noggit/Misc.h>
#include <noggit/TileWater.hpp>
#include <noggit/liquid_layer.hpp>
#include <noggit/World.h>
#include <noggit/rendering/LiquidTextureManager.hpp>
#include <ClientFile.hpp>

#include <stdexcept>
#include <limits>

TileWater::TileWater(MapTile *pTile, float pXbase, float pZbase, bool use_mclq_green_lava)
  : tile(pTile)
  , _renderer(pTile)
  , xbase(pXbase)
  , zbase(pZbase)
  , _extents{glm::vec3{pXbase, std::numeric_limits<float>::max(), pZbase},
            glm::vec3{pXbase + TILESIZE, std::numeric_limits<float>::lowest(), pZbase + TILESIZE}}
{
  for (int z = 0; z < 16; ++z)
  {
    for (int x = 0; x < 16; ++x)
    {
      chunks[z][x] = std::make_unique<ChunkWater> (tile->getChunk(z, x), this, xbase + CHUNKSIZE * x, zbase + CHUNKSIZE * z, use_mclq_green_lava);
    }
  }
}

void TileWater::readFromFile(BlizzardArchive::ClientFile &theFile, size_t basePos)
{
  for (int z = 0; z < 16; ++z)
  {
    for (int x = 0; x < 16; ++x)
    {
      theFile.seek(basePos + (z * 16 + x) * sizeof(MH2O_Header));
      chunks[z][x]->fromFile(theFile, basePos);
    }
  }
}


ChunkWater* TileWater::getChunk(int x, int z)
{
  return chunks[z][x].get();
}

void TileWater::autoGen(float factor)
{
  for (int z = 0; z < 16; ++z)
  {
    for (int x = 0; x < 16; ++x)
    {
      chunks[z][x]->autoGen(tile->getChunk(x, z), factor);
    }
  }
}

void TileWater::saveToFile(sExtendableArray &lADTFile, int &lMHDR_Position, int &lCurrentPosition)
{
  if (!hasData(0))
  {
    return;
  }

  int ofsW = lCurrentPosition + 0x8; //water Header pos

  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mh2o = lCurrentPosition - 0x14; //setting offset to MH2O data in Header

  int headers_size = 256 * sizeof(MH2O_Header);
  // 8 empty bytes for the chunk header
  lADTFile.Extend(8 + headers_size);
  // set current pos after the mh2o headers
  lCurrentPosition = ofsW + headers_size;
  int header_pos = ofsW;


  for (int z = 0; z < 16; ++z)
  {
    for (int x = 0; x < 16; ++x)
    {
      chunks[z][x]->save(lADTFile, ofsW, header_pos, lCurrentPosition);
    }
  }

  SetChunkHeader(lADTFile, ofsW - 8, 'MH2O', lCurrentPosition - ofsW);
}

bool TileWater::hasData(size_t layer)
{
  for (int z = 0; z < 16; ++z)
  {
    for (int x = 0; x < 16; ++x)
    {
      if (chunks[z][x]->hasData(layer))
      {
        return true;
      }
    }
  }

  return false;
}

void TileWater::CropMiniChunk(int x, int z, MapChunk* chunkTerrain)
{
  chunks[z][x]->CropWater(chunkTerrain);
}

void TileWater::setType(int type, size_t layer)
{
  for (int z = 0; z < 16; ++z)
  {
    for (int x = 0; x < 16; ++x)
    {
      chunks[z][x]->setType(type, layer);
    }
  }
}

int TileWater::getType(size_t layer)
{
  for (int z = 0; z < 16; ++z)
  {
    for (int x = 0; x < 16; ++x)
    {
      if (chunks[z][x]->hasData(layer))
      {
        return chunks[z][x]->getType(layer);
      }
    }
  }
  return 0;
}

void TileWater::recalcExtents()
{
  _extents = {glm::vec3{xbase, std::numeric_limits<float>::max(), zbase},
              glm::vec3{xbase + TILESIZE, std::numeric_limits<float>::lowest(), zbase + TILESIZE}};

  for (int i = 0; i < 256; ++i)
  {
    int x = i / 16;
    int z = i % 16;

    auto& chunk = chunks[x][z];
    _extents[0].y = std::min(_extents[0].y, chunk->getMinHeight());
    _extents[1].y = std::max(_extents[1].y, chunk->getMaxHeight());
  }

  tile->tagCombinedExtents(true);
  tagExtents(false);
}

bool TileWater::isVisible(const math::frustum& frustum) const
{
  return frustum.intersects(_extents[1], _extents[0]);
}
