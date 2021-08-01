// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#version 330 core

layout (std140) uniform lighting
{
  vec4 DiffuseColor_FogStart;
  vec4 AmbientColor_FogEnd;
  vec4 FogColor_FogOn;
  vec4 LightDir_FogRate;
  vec4 OceanColorLight;
  vec4 OceanColorDark;
  vec4 RiverColorLight;
  vec4 RiverColorDark;
};

layout (std140) uniform overlay_params
{
  int draw_shadows;
  int draw_lines;
  int draw_hole_lines;
  int draw_areaid_overlay;
  int draw_terrain_height_contour;
  int draw_wireframe;
  int wireframe_type;
  float wireframe_radius;
  float wireframe_width;
  vec4 wireframe_color;
};

uniform sampler2D shadow_map;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D stampBrush;
uniform vec2 tex_anim_0;
uniform vec2 tex_anim_1;
uniform vec2 tex_anim_2;
uniform vec2 tex_anim_3;
uniform sampler2D alphamap;
uniform int layer_count;
uniform bool has_mccv;
uniform bool cant_paint;

uniform vec4 areaid_color;
uniform bool draw_impassible_flag;

uniform bool draw_selection;

uniform vec3 camera;

uniform int draw_cursor_circle;
uniform vec3 cursor_position;
uniform float cursorRotation;
uniform float outer_cursor_radius;
uniform float inner_cursor_ratio;
uniform vec4 cursor_color;

in vec3 vary_position;
in vec2 vary_texcoord;
in vec3 vary_normal;
in vec3 vary_mccv;

out vec4 out_color;

const float TILESIZE  = 533.33333;
const float CHUNKSIZE = TILESIZE / 16.0;
const float HOLESIZE  = CHUNKSIZE * 0.25;
const float UNITSIZE = HOLESIZE * 0.5;
const float PI = 3.14159265358979323846;

vec4 texture_blend() 
{
  vec3 alpha = texture(alphamap, vary_texcoord / 8.0).rgb;
  float a0 = alpha.r;  
  float a1 = alpha.g;
  float a2 = alpha.b;

  vec3 t0 = texture(tex0, vary_texcoord + tex_anim_0).rgb;
  vec3 t1 = texture(tex1, vary_texcoord + tex_anim_1).rgb;
  vec3 t2 = texture(tex2, vary_texcoord + tex_anim_2).rgb;
  vec3 t3 = texture(tex3, vary_texcoord + tex_anim_3).rgb;

  return mix(vec4 (1.0, 1.0, 1.0, 1.0), vec4 (t0 * (1.0 - (a0 + a1 + a2)) + t1 * a0 + t2 * a1 + t3 * a2, 1.0), int(layer_count > 0));
}

float contour_alpha(float unit_size, float pos, float line_width)
{
  float f = abs(fract((pos + unit_size*0.5) / unit_size) - 0.5);
  float df = abs(line_width / unit_size);
  return smoothstep(0.0, df, f);
}

float contour_alpha(float unit_size, vec2 pos, vec2 line_width)
{
  return 1.0 - min( contour_alpha(unit_size, pos.x, line_width.x)
                  , contour_alpha(unit_size, pos.y, line_width.y)
                  );
}

void main()
{
  float dist_from_camera = distance(camera, vary_position);

  vec3 fw = fwidth(vary_position.xyz);

  out_color = texture_blend();

  out_color.rgb *= vary_mccv;

  // apply world lighting
  vec3 currColor;
  vec3 lDiffuse = vec3(0.0, 0.0, 0.0);
  vec3 accumlatedLight = vec3(1.0, 1.0, 1.0);

  float nDotL = clamp(dot(normalize(vary_normal), -normalize(LightDir_FogRate.xyz)), 0.0, 1.0);

  vec3 skyColor = (AmbientColor_FogEnd.xyz * 1.10000002);
  vec3 groundColor = (AmbientColor_FogEnd.xyz * 0.699999988);

  currColor = mix(groundColor, skyColor, 0.5 + (0.5 * nDotL));
  lDiffuse = DiffuseColor_FogStart.xyz * nDotL;

  out_color.rgb = clamp(out_color.rgb * (currColor + lDiffuse), 0.0, 1.0);

  // apply overlays
  if(cant_paint)
  {
    out_color *= vec4(1.0, 0.0, 0.0, 1.0);
  }
  
  if(draw_areaid_overlay != 0)
  {
    out_color = out_color * 0.3 + areaid_color;
  }

  if(draw_impassible_flag)
  {
    out_color.rgb = mix(vec3(1.0), out_color.rgb, 0.5);
  }

  if(draw_selection)
  {
   out_color.rgb = mix(vec3(1.0), out_color.rgb, 0.5);
  }

  if (draw_shadows != 0)
  {
    float shadow_alpha = texture(shadow_map, vary_texcoord / 8.0).r;
    out_color = vec4 (out_color.rgb * (1.0 - shadow_alpha), 1.0);
  }

  if (draw_terrain_height_contour != 0)
  {
    out_color = vec4(out_color.rgb * contour_alpha(4.0, vary_position.y+0.1, fw.y), 1.0);
  }

  bool lines_drawn = false;
  if(draw_lines != 0)
  {
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

    color.a = contour_alpha(TILESIZE, vary_position.xz, fw.xz * 1.5);
    color.g = color.a > 0.0 ? 0.8 : 0.0;

    if(color.a == 0.0)
    {
      color.a = contour_alpha(CHUNKSIZE, vary_position.xz, fw.xz);
      color.r = color.a > 0.0 ? 0.8 : 0.0;
    }
    if(draw_hole_lines != 0 && color.a == 0.0)
    {
      color.a = contour_alpha(HOLESIZE, vary_position.xz, fw.xz * 0.75);
      color.b = 0.8;
    }
    
    lines_drawn = color.a > 0.0;
    out_color.rgb = mix(out_color.rgb, color.rgb, color.a);
  }

  if(FogColor_FogOn.w != 0)
  {
    float start = AmbientColor_FogEnd.w * DiffuseColor_FogStart.w; // 0

    vec3 fogParams;
    fogParams.x = -(1.0 / (AmbientColor_FogEnd.w - start)); // - 1 / 338
    fogParams.y = (1.0 / (AmbientColor_FogEnd.w - start)) * AmbientColor_FogEnd.w; // 1 / 338 * 338
    fogParams.z = LightDir_FogRate.w; // 2.7

    float f1 = (dist_from_camera * fogParams.x) + fogParams.y; // 1.0029
    float f2 = max(f1, 0.0);
    float f3 = pow(f2, fogParams.z);
    float f4 = min(f3, 1.0);

    float fogFactor = 1.0 - f4;

    float alpha = clamp((dist_from_camera - start) / (AmbientColor_FogEnd.w - start), 0.0, 1.0);

    out_color.rgb = mix(out_color.rgb, FogColor_FogOn.rgb, alpha);
  }

  if(draw_wireframe != 0 && !lines_drawn)
  {
    // true by default => type 0
	  bool draw_wire = true;
    float real_wireframe_radius = max(outer_cursor_radius * wireframe_radius, 2.0 * UNITSIZE); 
	
	  if(wireframe_type == 1)
	  {
		  draw_wire = (length(vary_position.xz - cursor_position.xz) < real_wireframe_radius);
	  }
	
	  if(draw_wire)
	  {
		  float alpha = contour_alpha(UNITSIZE, vary_position.xz, fw.xz * wireframe_width);
		  float xmod = mod(vary_position.x, UNITSIZE);
		  float zmod = mod(vary_position.z, UNITSIZE);
		  float d = length(fw.xz) * wireframe_width;
		  float diff = min( min(abs(xmod - zmod), abs(xmod - UNITSIZE + zmod))
                      , min(abs(zmod - xmod), abs(zmod + UNITSIZE - zmod))
                      );        

		  alpha = max(alpha, 1.0 - smoothstep(0.0, d, diff));
      out_color.rgb = mix(out_color.rgb, wireframe_color.rgb, wireframe_color.a*alpha);
	  }
  }

  if(draw_cursor_circle == 1)
  {
    float diff = length(vary_position.xz - cursor_position.xz);
    diff = min(abs(diff - outer_cursor_radius), abs(diff - outer_cursor_radius * inner_cursor_ratio));
    float alpha = smoothstep(0.0, length(fw.xz), diff);

    out_color.rgb = mix(cursor_color.rgb, out_color.rgb, alpha);
  }
  else if(draw_cursor_circle == 2)
  {
    float angle = cursorRotation * 2.0 * PI;
    vec2 topleft = cursor_position.xz;
    topleft.x -= outer_cursor_radius;
    topleft.y -= outer_cursor_radius;
    vec2 texcoord = (vary_position.xz - topleft) / outer_cursor_radius * 0.5 - 0.5;
    vec2 rotatedTexcoord;
    rotatedTexcoord.x = texcoord.x * cos(angle) + texcoord.y * sin(angle) + 0.5;
    rotatedTexcoord.y = texcoord.y * cos(angle) - texcoord.x * sin(angle) + 0.5;
    /*out_color.rgb = mix(out_color.rgb, texture(stampBrush, rotatedTexcoord).rgb
    , 1.0 * (int(length(vary_position.xz - cursor_position.xz) / outer_cursor_radius < 1.0))
    * (1.0 - length(vary_position.xz - cursor_position.xz) / outer_cursor_radius));*/
    out_color.rgb = mix(out_color.rgb, cursor_color.rgb, texture(stampBrush, rotatedTexcoord).r
    * int(abs(vary_position.x - cursor_position.x) <= outer_cursor_radius
    && abs(vary_position.z - cursor_position.z) <= outer_cursor_radius));
    /*vec2 posRel = vary_position.xz - cursor_position.xz;
    float pos_x = posRel.x * sin(angle) - posRel.y * cos(angle);
    float pos_z = posRel.y * sin(angle) + posRel.x * cos(angle);
    float diff_x = abs(pos_x);
    float diff_z = abs(pos_z);
    float inner_radius = outer_cursor_radius * inner_cursor_ratio;
    float d = length(fw);
    float alpha = 1.0 * (1 - int((diff_x < outer_cursor_radius && diff_z < outer_cursor_radius
    && (outer_cursor_radius - diff_x <= d || outer_cursor_radius - diff_z <= d)) || (diff_x < inner_radius
    && diff_z < inner_radius && (inner_radius - diff_x <= d || inner_radius - diff_z <= d))));
    out_color.rgb = mix(cursor_color.rgb, out_color.rgb, alpha);*/
  }
}
