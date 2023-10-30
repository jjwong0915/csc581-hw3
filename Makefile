OBJS = src/model/character/character.o src/model/death_zone/death_zone.o src/model/moving_platform/moving_platform.o src/model/static_platform/static_platform.o src/model/side_boundary/side_boundary.o src/model/spawn_point/spawn_point.o src/sfml_addon/json/json.o
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lzmq -lpthread
CXXFLAGS = -std=c++17 -Isrc -Isrc/model

all: server client server_rr client_rr

server: src/program/server/server.o $(OBJS)
	$(CXX) -o server src/program/server/server.o $(OBJS) $(LIBS)

client: src/program/client/client.o $(OBJS)
	$(CXX) -o client src/program/client/client.o $(OBJS) $(LIBS)

server_rr: src/program_rr/server_rr/server_rr.o $(OBJS)
	$(CXX) -o server_rr src/program_rr/server_rr/server_rr.o $(OBJS) $(LIBS)

client_rr: src/program_rr/client_rr/client_rr.o $(OBJS)
	$(CXX) -o client_rr src/program_rr/client_rr/client_rr.o $(OBJS) $(LIBS)

####

src/model/character/character.o: src/model/character/character.hpp

src/model/death_zone/death_zone.o: src/model/death_zone/death_zone.hpp

src/model/side_boundary/side_boundary.o: src/model/side_boundary/side_boundary.hpp

src/model/spawn_point/spawn_point.o: src/model/spawn_point/spawn_point.hpp

src/model/static_platform/static_platform.o: src/model/static_platform/static_platform.hpp src/sfml_addon/json/json.hpp

src/sfml_addon/json/json.o: src/sfml_addon/json/json.hpp


src/program/server/server.o: src/model/character/character.hpp src/model/death_zone/death_zone.hpp src/model/moving_platform/moving_platform.hpp src/sfml_addon/json/json.hpp src/model/spawn_point/spawn_point.hpp src/model/static_platform/static_platform.hpp

src/program/client/client.o: src/model/character/character.hpp  src/model/moving_platform/moving_platform.hpp src/model/static_platform/static_platform.hpp src/model/side_boundary/side_boundary.hpp


src/program_rr/server_rr/server_rr.o: src/model/character/character.hpp src/model/death_zone/death_zone.hpp src/model/moving_platform/moving_platform.hpp src/sfml_addon/json/json.hpp src/model/spawn_point/spawn_point.hpp src/model/static_platform/static_platform.hpp

src/program_rr/client_rr/client_rr.o: src/model/character/character.hpp  src/model/moving_platform/moving_platform.hpp src/model/static_platform/static_platform.hpp src/model/side_boundary/side_boundary.hpp

####

.PHONY: clean
clean:
	rm server client $(OBJS)
