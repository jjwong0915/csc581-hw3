#include <SFML/Graphics.hpp>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <vector>
#include <zmq.hpp>

#include "character/character.hpp"
#include "death_zone/death_zone.hpp"
#include "moving_platform/moving_platform.hpp"
#include "sfml_addon/json/json.hpp"
#include "spawn_point/spawn_point.hpp"
#include "static_platform/static_platform.hpp"

using json = nlohmann::json;

std::mutex object_mtx;
// static platforms
static_platform platform_left{{200, 30}, {0, 500}, sf::Color::Blue};
static_platform platform_right{{200, 30}, {600, 500}, sf::Color::Blue};
static_platform platform_up{{200, 30}, {900, 100}, sf::Color::Red};
// moving platforms
moving_platform platform_vertical({800, 500}, {0, -1}, sf::Color::Yellow);
moving_platform platform_horizontal({400, 400}, {2, 0}, sf::Color::Cyan);
// death zone
death_zone bottom_zone{{-1000, 600}, {2900, 50}};
// spawn points
std::deque<spawn_point> spawn_points;
// characters
std::map<unsigned, character> client_list;

void handle_object() {
  zmq::context_t context;
  zmq::socket_t obj_socket(context, ZMQ_REP);
  obj_socket.bind("tcp://127.0.0.1:5555");
  //
  while (true) {
    zmq::message_t message;
    zmq::recv_result_t result = obj_socket.recv(message, zmq::recv_flags::none);
    //
    std::lock_guard<std::mutex> object_lck{object_mtx};
    json obj_json = {platform_left, platform_right, platform_up,
                     platform_vertical, platform_horizontal};
    for (auto& id_char : client_list) {
      obj_json.push_back(id_char.second);
    }
    std::string obj_str = obj_json.dump();
    obj_socket.send(zmq::message_t{obj_str.begin(), obj_str.end()},
                    zmq::send_flags::none);
  }
}

void handle_message() {
  zmq::context_t context;
  zmq::socket_t msg_socket(context, ZMQ_REP);
  msg_socket.bind("tcp://127.0.0.1:5556");
  //
  while (true) {
    zmq::message_t message;
    zmq::recv_result_t result = msg_socket.recv(message, zmq::recv_flags::none);
    //
    json message_json = json::parse(message.to_string());
    std::string message_type = message_json.at("type");
    unsigned client_id = message_json.at("client_id");
    std::lock_guard<std::mutex> object_lck{object_mtx};
    if (message_type == "join") {
      client_list[client_id] =
          character{client_id, {80, 400}, {0, 0}, sf::Color::White};
    } else if (message_type == "leave") {
      client_list.erase(client_id);
    } else if (message_type == "go_left") {
      client_list[client_id].go_left();
    } else if (message_type == "go_right") {
      client_list[client_id].go_right();
    } else if (message_type == "jump") {
      client_list[client_id].jump();
    }
    //
    msg_socket.send(zmq::message_t{}, zmq::send_flags::none);
  }
}

int main() {
  // initialize spawn points
  spawn_points.push_back(sf::Vector2f{60, 400});
  spawn_points.push_back(sf::Vector2f{80, 400});
  spawn_points.push_back(sf::Vector2f{100, 400});
  // object handler
  std::cout << "starting object handler" << std::endl;
  std::thread object_handler(handle_object);
  // message handler
  std::cout << "starting message handler" << std::endl;
  std::thread message_handler(handle_message);
  // server loop
  std::cout << "starting server loop" << std::endl;
  while (true) {
    // locking client list
    {
      std::lock_guard<std::mutex> object_lck{object_mtx};
      // apply gravitational accelration
      for (auto& id_char : client_list) {
        id_char.second.apply_gravity();
      }
      // apply object collision
      for (auto& id_char : client_list) {
        character& character_ = id_char.second;
        character_.reset_collision();
        character_.apply_collision(platform_left.get_shape().getGlobalBounds());
        character_.apply_collision(
            platform_right.get_shape().getGlobalBounds());
        character_.apply_collision(platform_up.get_shape().getGlobalBounds());
        character_.apply_collision(
            platform_vertical.get_shape().getGlobalBounds());
        character_.apply_collision(
            platform_horizontal.get_shape().getGlobalBounds());
      }
      // execute object movement
      for (auto& id_char : client_list) {
        id_char.second.move();
      }
      platform_vertical.move();
      platform_horizontal.move();
      // apply character death
      for (auto& id_char : client_list) {
        id_char.second.reset_death();
        id_char.second.apply_death(bottom_zone.get_bound(), spawn_points);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{10});
  }
}
