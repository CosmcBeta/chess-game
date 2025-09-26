#include "theme.hpp"

Theme::Theme()
    :darkBrown_(181, 136, 95, 255), lightBrown_(143, 107, 74, 255), altBrown_(240, 217, 181, 255),
    darkGreen_(115, 149, 82, 255), lightGreen_(140, 183, 105, 255), altGreen_(235, 236, 208, 255),
    darkBlue_(140, 162, 173, 255), lightBlue_(170, 190, 200, 255), altBlue_(222, 227, 230, 255),
    moveCircle(140, 140, 140, 160)
{
    setTheme(ThemeSet::Brown);
}

void Theme::setTheme(ThemeSet p_theme)
{
    switch (p_theme)
    {
    case ThemeSet::Brown:
        darkMain = darkBrown_;
        lightMain = lightBrown_;
        alternate = altBrown_;
        break;
    case ThemeSet::Green:
        darkMain = darkGreen_;
        lightMain = lightGreen_;
        alternate = altGreen_;
        break;
    case ThemeSet::Blue:
        darkMain = darkBlue_;
        lightMain = lightBlue_;
        alternate = altBlue_;
        break;
    default:
        break;
    }
}