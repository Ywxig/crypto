#ifndef COLORISE_HPP
#define COLORISE_HPP

#include <string>

std::string echo(const std::string& message);
std::string print(const std::string& message);
void color(const std::string& colorCode);
void resetColor();

#endif // COLORISE_HPP
