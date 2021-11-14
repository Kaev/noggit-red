// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#pragma once
#include <glm/vec3.hpp>
#include <noggit/scripting/script_object.hpp>
#include <noggit/scripting/script_image.hpp>

class MapChunk;

namespace noggit {
  namespace scripting {
    class scripting_tool;
    class script_context;

    class tex: public script_object
    {
    public:
      tex(script_context * ctx, MapChunk* chunk, int index);
      void set_alpha(int index, float alpha);
      float get_alpha(int index);
      glm::vec3 get_pos_2d();
    
    private:
      MapChunk* _chunk;
      int _index;
    };

    void collect_textures(
        script_context* ctx
      , MapChunk* chnk
      , std::vector<tex>& vec
      , glm::vec3 const& min
      , glm::vec3 const& max
    );

    class tex_iterator : public script_object {
      public:
        tex_iterator( script_context * ctx
                    , std::shared_ptr<std::vector<MapChunk*>> chunks
                    , glm::vec3 const& min
                    , glm::vec3 const& max
                    );
        bool next();
        tex get();

        void paint_image(image & img, int layer, float pressure, float angle);

      private:
        int _tex_iter = -1;
        std::shared_ptr<std::vector<MapChunk*>> _chunks;
        std::vector<MapChunk*>::iterator _chunk_iter;
        glm::vec3 const& _min;
        glm::vec3 const& _max;
    };

    void register_tex(script_context * state);
  }
}