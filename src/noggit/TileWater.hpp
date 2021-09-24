// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/vector_3d.hpp>
#include <math/vector_4d.hpp>
#include <noggit/ChunkWater.hpp>
#include <noggit/MPQ.h>
#include <noggit/MapHeaders.h>
#include <noggit/tool_enums.hpp>
#include <opengl/context.hpp>
#include <opengl/types.hpp>
#include <noggit/LiquidTextureManager.hpp>

#include <memory>
#include <vector>
#include <array>
#include <set>

class MapTile;
class liquid_layer;
class sExtendableArray;
enum LiquidLayerUpdateFlags;


struct LiquidLayerDrawCallData
{
  unsigned n_used_chunks = 0;
  std::array<opengl::LiquidChunkInstanceDataUniformBlock, 256> chunk_data;
  std::array<std::array<math::vector_4d, 9 * 9>, 256> vertex_data ;
  std::vector<int> texture_samplers;
  GLuint chunk_data_buf = 0;
  GLuint vertex_data_tex = 0;
  bool update_tag = true;
  bool remove_tag = false;
  bool upload_tag = false;
};

class TileWater
{

public:
  TileWater(MapTile *pTile, float pXbase, float pZbase, bool use_mclq_green_lava);

  ChunkWater* getChunk(int x, int z);

  void readFromFile(MPQFile &theFile, size_t basePos);
  void saveToFile(sExtendableArray &lADTFile, int &lMHDR_Position, int &lCurrentPosition);

  void draw ( math::frustum const& frustum
            , const float& cull_distance
            , const math::vector_3d& camera
            , bool camera_moved
            , opengl::scoped::use_program& water_shader
            , int animtime
            , int layer
            , display_mode display
            , LiquidTextureManager* tex_manager
            );
  bool hasData(size_t layer);
  void CropMiniChunk(int x, int z, MapChunk* chunkTerrain);

  void autoGen(float factor);

  void setType(int type, size_t layer);
  int getType(size_t layer);

  void registerNewChunk(std::size_t layer);
  void unregisterChunk(std::size_t layer);
  void updateLayer(std::size_t layer);

  void updateLayerData(LiquidTextureManager* tex_manager);

  void unload();

  void upload();

private:

  MapTile *tile;

  std::unique_ptr<ChunkWater> chunks[16][16];

  std::vector<LiquidLayerDrawCallData> _render_layers;

  bool _need_buffer_update = false;
  bool _loaded = false;

  float xbase;
  float zbase;
};
