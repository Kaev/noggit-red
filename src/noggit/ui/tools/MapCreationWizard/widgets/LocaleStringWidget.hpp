#include <QComboBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QWidget>
#include <QLineEdit>
#include <noggit/types/NoggitTypes.h>

namespace Noggit::Ui::Tools::MapCreationWizard::Widget
{
    class LocaleStringWidget : public QWidget
    {
    public:
        LocaleStringWidget(QWidget* parent = nullptr);

        void setCurrentLocale(const std::string& locale);
        void setValue(const std::string& val, int locale) const;
        std::string getValue(int locale) const;
        void fill(LocaleString& record);
        void clear();
        void toRecord(LocaleString& record);

    private:
        QComboBox* _current_locale;
        QStackedWidget* _show_entry;

        std::unordered_map<std::string, QLineEdit*> _widget_map;

        QSpinBox* _flags;
    };
}