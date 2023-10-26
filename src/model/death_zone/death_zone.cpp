#include "death_zone/death_zone.hpp"

death_zone::death_zone() : position{0, 0}, size{0, 0} {}

death_zone::death_zone(sf::Vector2f pos, sf::Vector2f siz)
    : position{pos}, size{siz} {}

sf::FloatRect death_zone::get_bound() { return sf::FloatRect{position, size}; }
