// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/ClickableLabel.hpp>

namespace Noggit
{
  namespace Ui
  {
    ClickableLabel::ClickableLabel(QWidget * parent) : QLabel(parent){}

    void ClickableLabel::mouseReleaseEvent (QMouseEvent*)
    {
      emit clicked();
    }
  }
}
