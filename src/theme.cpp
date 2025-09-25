#include "Theme.hpp"

Theme::Theme()
    :darkBrown(181, 136, 95, 255), lightBrown(143, 107, 74, 255), altBrown(240, 217, 181, 255),
    darkGreen(115, 149, 82, 255), lightGreen(140, 183, 105, 255), altGreen(235, 236, 208, 255),
    darkBlue(140, 162, 173, 255), lightBlue(170, 190, 200, 255), altBlue(222, 227, 230, 255)
{
    setTheme(ThemeSet::BROWN);
}

void Theme::setTheme(ThemeSet p_theme)
{
    switch (p_theme)
    {
    case ThemeSet::BROWN:
        darkMain = darkBrown;
        lightMain = lightBrown;
        alternate = altBrown;
        break;
    case ThemeSet::GREEN:
        darkMain = darkGreen;
        lightMain = lightGreen;
        alternate = altGreen;
        break;
    case ThemeSet::BLUE:
        darkMain = darkBlue;
        lightMain = lightBlue;
        alternate = altBlue;
        break;
    default:
        break;
    }
}