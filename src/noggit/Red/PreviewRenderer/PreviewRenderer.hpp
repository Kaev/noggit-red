#ifndef NOGGIT_PREVIEWRENDERER_HPP
#define NOGGIT_PREVIEWRENDERER_HPP

#include <math/matrix_4x4.hpp>
#include <math/vector_3d.hpp>
#include <noggit/camera.hpp>
#include <noggit/WMOInstance.h>
#include <noggit/ModelInstance.h>
#include <noggit/WMO.h>
#include <noggit/Model.h>
#include <noggit/ContextObject.hpp>
#include <noggit/bool_toggle_property.hpp>

#include <QOpenGLWidget>
#include <QSettings>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObjectFormat>
#include <QOffscreenSurface>
#include <QPixmap>

#include <vector>


namespace noggit::Red
{

  class PreviewRenderer : public QOpenGLWidget
  {
    Q_OBJECT

  public:
    explicit PreviewRenderer(int width, int height, noggit::NoggitRenderContext context, QWidget* parent = nullptr);

    void resetCamera(float x = 0.f, float y = 0.f, float z = 0.f, float roll = 0.f, float yaw = 120.f, float pitch = 20.f);
    QPixmap* renderToPixmap();

    virtual void setModel(std::string const& filename);
    void setModelOffscreen(std::string const& filename);
    virtual void setPrefab(std::string const& filename) {};

    void setLightDirection(float y, float z);

    bool_toggle_property _draw_models = {true};
    bool_toggle_property _draw_wmo = {true};
    bool_toggle_property _draw_particles = {true};
    bool_toggle_property _draw_animated = {true};
    bool_toggle_property _draw_boxes = {false};

  protected:

    noggit::camera _camera;
    QSettings* _settings;
    std::string _filename;

    std::unique_ptr<opengl::program> _m2_program;
    std::unique_ptr<opengl::program> _m2_instanced_program;
    std::unique_ptr<opengl::program> _m2_particles_program;
    std::unique_ptr<opengl::program> _m2_ribbons_program;
    std::unique_ptr<opengl::program> _m2_box_program;
    std::unique_ptr<opengl::program> _wmo_program;

    boost::optional<liquid_render> _liquid_render = boost::none;

    std::vector<ModelInstance> _model_instances;
    std::vector<WMOInstance> _wmo_instances;

    float _animtime = 0.f;

    std::vector<math::vector_3d> calcSceneExtents();
    virtual void draw();
    virtual void tick(float dt);
    virtual math::matrix_4x4 model_view() const;
    virtual math::matrix_4x4 projection() const;
    virtual float aspect_ratio() const;

    void update_emitters(float dt);

  private:
    int _width;
    int _height;
    noggit::NoggitRenderContext _context;

    std::map<std::tuple<std::string, int, int>, QPixmap> _cache;

    QOpenGLContext _offscreen_context;
    QOpenGLFramebufferObjectFormat _fmt;
    QOffscreenSurface _offscreen_surface;

    math::vector_3d _background_color;
    math::vector_3d _diffuse_light;
    math::vector_3d _ambient_light;
    math::vector_3d _light_dir;

  };

}


#endif //NOGGIT_PREVIEWRENDERER_HPP
