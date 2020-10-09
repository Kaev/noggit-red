// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/interpolation.hpp>
#include <math/vector_4d.hpp>

#include <cstdint>

namespace math
{
  struct vector_3d;

  //! \note Actually, a typedef would be enough.
  struct quaternion : public vector_4d
  {
  public:
    quaternion()
      : quaternion (0.f, 0.f, 0.f, 1.0f)
    {}

   quaternion ( const float& x
              , const float& y
              , const float& z
              , const float& w
              )
     : vector_4d (x, y, z, w)
   { }

   explicit quaternion (const vector_4d& v)
     : vector_4d (v)
   { }

   quaternion (const vector_3d& v, const float w)
     : vector_4d (v, w)
   { }
  };

  //! \note "linear" interpolation for quaternions should be slerp by default.
  namespace interpolation
  {
    template<>
    inline quaternion linear ( const float& percentage
                      , const quaternion& start
                      , const quaternion& end
                      )
    {
      return slerp (percentage, start, end);
    }
  }

  //! \note In WoW 2.0+ Blizzard is now storing rotation data in 16bit values instead of 32bit. I don't really understand why as its only a very minor saving in model sizes and adds extra overhead in processing the models. Need this structure to read the data into.
  struct packed_quaternion
  {
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t w;
  };
}
