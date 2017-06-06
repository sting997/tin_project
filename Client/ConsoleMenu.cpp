//
// Created by riuoku on 29.05.17.
//

#include "ConsoleMenu.h"

std::ostream &ConsoleMenu::display() {
    std::list<std::string>::const_iterator it = options.begin();

    while (it != options.end())
        std::cout << *it++ << '\n';

    return std::cout << msg << std::flush;
}

bool ConsoleMenu::selection() {
    if (std::cin >> option && std::find(values.begin(), values.end(), option) != values.end())
        return true;

    if (!std::cin.good()) {
        std::cin.clear();

        int ch;
        while ((ch = std::cin.get()) != '\n' && ch != EOF);
    }

    return false;
}
