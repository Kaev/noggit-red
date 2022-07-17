// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/windows/LoadingScreenViewer/LoadingScreenViewer.h>
#include <noggit/TextureManager.h>
#include "revision.h"
#include <noggit/Log.h>
#include <sstream>
#include <map>

namespace Noggit::Ui::Windows
{
    LoadingScreenViewer::LoadingScreenViewer(std::shared_ptr<Project::NoggitProject> project, std::string loadingScreenPath, QWidget* parent) : QMainWindow(parent)
        , ui(new ::Ui::LoadingScreenViewer), _project(project),_filePath(loadingScreenPath)
    {
        ui->setupUi(this);

        auto pixelMap = Noggit::BLPRenderer::getInstance().render_blp_to_pixmap(loadingScreenPath);

        ui->label->setFixedWidth(1024);
        ui->label->setFixedHeight(1024);

        setFixedWidth(1024);
        setFixedHeight(1024);

        ui->label->setPixmap(pixelMap->scaled(1024, 1024, Qt::KeepAspectRatio));

    }
}
