#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include <zmq.hpp>

#include "character/character.hpp"
#include "moving_platform/moving_platform.hpp"
#include "side_boundary/side_boundary.hpp"
#include "static_platform/static_platform.hpp"

using json = nlohmann::json;

int main() {
  unsigned client_id = std::random_device{}();
  // setup sockets
  zmq::context_t context;
  zmq::socket_t obj_socket(context, ZMQ_REQ);
  obj_socket.connect("tcp://127.0.0.1:5555");
  zmq::socket_t msg_socket(context, ZMQ_REQ);
  msg_socket.connect("tcp://127.0.0.1:5556");
  // send join message
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  std::string msg_str = json{{"type", "join"}, {"client_id", client_id}}.dump();
  msg_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                  zmq::send_flags::none);
  zmq::message_t message;
  zmq::recv_result_t result = msg_socket.recv(message, zmq::recv_flags::none);
  // side boundaries
  side_boundary boundary_left{{0, -1000}, {100, 1600}};
  side_boundary boundary_right{{700, -1000}, {100, 1600}};
  // pressed keys
  std::set<sf::Keyboard::Key> pressed_keys;
  // performance counter
  auto last_time = std::chrono::high_resolution_clock::now();
  unsigned loop_since_last_time = 0;
  // event and display loop
  sf::RenderWindow window(sf::VideoMode{800, 600}, "Homework 3",
                          sf::Style::Close);
  while (window.isOpen()) {
    // handle event and keyboard
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        std::string msg_str =
            json{{"type", "leave"}, {"client_id", client_id}}.dump();
        msg_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                        zmq::send_flags::none);
        zmq::message_t message;
        zmq::recv_result_t result =
            msg_socket.recv(message, zmq::recv_flags::none);
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        pressed_keys.insert(event.key.code);
        if (event.key.code == sf::Keyboard::Space) {
          std::string msg_str =
              json{{"type", "jump"}, {"client_id", client_id}}.dump();
          msg_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                          zmq::send_flags::none);
          zmq::message_t message;
          zmq::recv_result_t result =
              msg_socket.recv(message, zmq::recv_flags::none);
        }
      } else if (event.type == sf::Event::KeyReleased) {
        pressed_keys.erase(event.key.code);
      }
    }
    if (pressed_keys.count(sf::Keyboard::Left) > 0) {
      std::string msg_str =
          json{{"type", "go_left"}, {"client_id", client_id}}.dump();
      msg_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                      zmq::send_flags::none);
      zmq::message_t message;
      zmq::recv_result_t result =
          msg_socket.recv(message, zmq::recv_flags::none);
    }
    if (pressed_keys.count(sf::Keyboard::Right) > 0) {
      std::string msg_str =
          json{{"type", "go_right"}, {"client_id", client_id}}.dump();
      msg_socket.send(zmq::message_t{msg_str.begin(), msg_str.end()},
                      zmq::send_flags::none);
      zmq::message_t message;
      zmq::recv_result_t result =
          msg_socket.recv(message, zmq::recv_flags::none);
    }
    // draw objects
    window.clear();
    zmq::message_t message;
    obj_socket.send(message, zmq::send_flags::none);
    zmq::recv_result_t result = obj_socket.recv(message, zmq::recv_flags::none);
    for (json& obj : json::parse(message.to_string())) {
      if (obj.at("type") == "moving_platform") {
        moving_platform platform{obj};
        window.draw(platform.get_shape());
      }
      if (obj.at("type") == "static_platform") {
        static_platform platform{obj};
        window.draw(platform.get_shape());
      }
      if (obj.at("type") == "character") {
        character character_{obj};
        window.draw(character_.get_shape());
        if (character_.get_clinet_id() == client_id) {
          if (character_.is_respawned()) {
            sf::View view = window.getDefaultView();
            view.setCenter(character_.get_position().x + 300,
                           view.getCenter().y);
            window.setView(view);
          }
          character_.apply_scrolling(window, boundary_left.get_bound());
          character_.apply_scrolling(window, boundary_right.get_bound());
        }
      }
    }
    window.display();
    //
    loop_since_last_time += 1;
    if (loop_since_last_time == 1000) {
      auto total_time = std::chrono::high_resolution_clock::now() - last_time;
      std::cout << "total time of last 1000 loops: "
                << (double)total_time.count() / 1000000000 << " s\n";
      loop_since_last_time = 0;
      last_time = std::chrono::high_resolution_clock::now();
    }
  }
}