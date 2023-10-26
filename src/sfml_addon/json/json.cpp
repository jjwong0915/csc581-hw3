#include "sfml_addon/json/json.hpp"

#include <iostream>

void sf::to_json(json& j, const sf::Vector2f& v) { j = json{v.x, v.y}; }

void sf::from_json(const json& j, sf::Vector2f& v) {
  j.at(0).get_to(v.x);
  j.at(1).get_to(v.y);
};

void sf::to_json(json& j, const sf::Color& c) { j = c.toInteger(); }

void sf::from_json(const json& j, sf::Color& c) {
  c = sf::Color(j.template get<unsigned int>());
};