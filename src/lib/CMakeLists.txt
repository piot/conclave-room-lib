# generated by cmake-generator
cmake_minimum_required(VERSION 3.16.3)

add_library(conclave-room-lib STATIC 
  connection_quality.c
  room.c
  room_connection.c
  room_connections.c)

include(Tornado.cmake)
set_tornado(conclave-room-lib)

target_include_directories(conclave-room-lib PUBLIC ../include)


target_link_libraries(conclave-room-lib PUBLIC 
  conclave-serialize
  flood
  imprint
  monotonic-time
  secure-random
  stats)

