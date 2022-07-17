#include <QHBoxLayout>
#include <noggit/ui/tools/MapCreationWizard/widgets/LocaleStringWidget.hpp>

namespace Noggit::Ui::Tools::MapCreationWizard::Widget
{
    LocaleStringWidget::LocaleStringWidget(QWidget* parent) : QWidget(parent)
    {
        auto layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        _show_entry = new QStackedWidget(this);

        _widget_map = {

            {Locale::enUS, new QLineEdit(this)},
            {Locale::koKR, new QLineEdit(this)},
            {Locale::frFR, new QLineEdit(this)},
            {Locale::deDE, new QLineEdit(this)},
            {Locale::zhCN, new QLineEdit(this)},
            {Locale::zhTW, new QLineEdit(this)},
            {Locale::esES, new QLineEdit(this)},
            {Locale::esMX, new QLineEdit(this)},
            {Locale::ruRU, new QLineEdit(this)},
            {Locale::jaJP, new QLineEdit(this)},
            {Locale::ptPT, new QLineEdit(this)},
            {Locale::itIT, new QLineEdit(this)},
            {Locale::Unk1, new QLineEdit(this)},
            {Locale::Unk2, new QLineEdit(this)},
            {Locale::Unk3, new QLineEdit(this)},
            {Locale::Unk4, new QLineEdit(this)}
        };

        for(auto widget : _widget_map)
        {
            _show_entry->addWidget(widget.second);
        }

        _flags = new QSpinBox(this);
        _flags->setVisible(false);

        layout->addWidget(_show_entry);

        _current_locale = new QComboBox(this);

        for (auto const& loc : LocaleNames)
        {
            _current_locale->addItem(QString::fromStdString(loc));
        }

        layout->addWidget(_current_locale);
        // Connect

        connect(_current_locale, &QComboBox::currentTextChanged
            , [&](QString s)
            {
                setCurrentLocale(_current_locale->currentText().toStdString());
            }
        );

        setMaximumHeight(_widget_map[0]->height());
    }

    void LocaleStringWidget::setCurrentLocale(const std::string& locale)
    {
        _show_entry->setCurrentWidget(_widget_map.at(locale));
    }

    void LocaleStringWidget::fill(Noggit::LocaleString& record)
    {
        for (int loc = 0; loc < 16; ++loc)
        {
            auto locale = LocaleNames[loc];
            setValue(record[locale], loc);
        }

        _flags->setValue(record.flags);
    }

    void LocaleStringWidget::toRecord(Noggit::LocaleString& record)
    {
        for (int loc = 0; loc < 16; ++loc)
        {
            auto locale = LocaleNames[loc];
            record.SetString(locale, getValue(loc));
        }
        record.flags = _flags->value();
    }

    void LocaleStringWidget::clear()
    {
        for (int loc = 0; loc < 16; ++loc)
        {
            setValue("", loc);
        }

        _flags->setValue(0);
    }

    void LocaleStringWidget::setValue(const std::string& val, int locale) const
    {
        _widget_map.at(LocaleNames[locale])->setText(QString::fromStdString(val));
    }

    std::string LocaleStringWidget::getValue(int locale) const
    {
        return  _widget_map.at(LocaleNames[locale])->text().toStdString();
    };

}