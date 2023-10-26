#include "character/character.hpp"

#include <algorithm>
#include <iostream>

#include "sfml_addon/json/json.hpp"

character::character()
    : client_id{0},
      position{0, 0},
      velocity{0, 0},
      color{0},
      supported{false},
      respawned{false} {};

character::character(unsigned cid, sf::Vector2f pos, sf::Vector2f vel,
                     sf::Color col)
    : client_id{cid},
      position{pos},
      velocity{vel},
      color{col},
      supported{false},
      respawned{false} {};

void character::go_left() { velocity.x = -2; }

void character::go_right() { velocity.x = 2; }

void character::jump() {
  if (supported) {
    velocity.y -= 7.5;
    supported = false;
  }
}

void character::move() {
  // velocity movement
  position.x += velocity.x;
  position.y += velocity.y;
  // no inertia on horizontal directions
  velocity.x = 0;
}

sf::RectangleShape character::get_shape() {
  sf::RectangleShape rectangle{{30, 30}};
  rectangle.setPosition(position);
  rectangle.setFillColor(color);
  return rectangle;
}

void character::apply_gravity() { velocity += {0, 0.2}; }

void character::reset_collision() { supported = false; }

void character::apply_collision(const sf::FloatRect& bound) {
  if (!supported) {
    float bottom = position.y + 30;
    if (bottom >= bound.top && bottom < bound.top + 10 &&
        position.x + 30 > bound.left && position.x < bound.left + bound.width) {
      position.y = bound.top - 30;
      velocity.y = std::min(0.f, velocity.y);
      supported = true;
    }
  }
}

void character::reset_death() { respawned = false; }

void character::apply_death(const sf::FloatRect& bound,
                            std::deque<spawn_point>& spawn_points) {
  if (sf::FloatRect{position, {30, 30}}.intersects(bound)) {
    position = spawn_points.front().get_position();
    velocity = {0, 0};
    std::rotate(spawn_points.begin(), spawn_points.begin() + 1,
                spawn_points.end());
    respawned = true;
  }
}

bool character::is_respawned() { return respawned; }

unsigned character::get_clinet_id() { return client_id; }

sf::Vector2f character::get_position() { return position; }

void character::apply_scrolling(sf::RenderWindow& window,
                                const sf::FloatRect& bound) {
  sf::View view = window.getView();
  sf::Vector2f default_center = window.getDefaultView().getCenter();
  float diff_x = default_center.x - view.getCenter().x;
  if (sf::FloatRect{{position.x + diff_x, position.y}, {30, 30}}.intersects(
          bound)) {
    if (position.x + diff_x + 30 < bound.left + bound.width) {
      view.setCenter(default_center +
                     sf::Vector2f{position.x + 30 - bound.left, 0});
    } else if (position.x + diff_x > bound.left) {
      view.setCenter(default_center -
                     sf::Vector2f{bound.left + bound.width - position.x, 0});
    }
    window.setView(view);
  }
}

void to_json(json& j, const character& c) {
  j = json{
      {"type", "character"},    {"client_id", c.client_id},
      {"position", c.position}, {"velocity", c.velocity},
      {"color", c.color},       {"respawned", c.respawned},
  };
}

void from_json(const json& j, character& c) {
  j.at("client_id").get_to(c.client_id);
  j.at("position").get_to(c.position);
  j.at("velocity").get_to(c.velocity);
  j.at("color").get_to(c.color);
  j.at("respawned").get_to(c.respawned);
}
