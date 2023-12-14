cmake_minimum_required(VERSION 3.16.3)
project(conclave-room-lib C)

add_subdirectory(deps/piot/clog/src/lib)
add_subdirectory(deps/piot/conclave-serialize-c/src/lib)
add_subdirectory(deps/piot/flood-c/src/lib)
add_subdirectory(deps/piot/guise-serialize-c/src/lib)
add_subdirectory(deps/piot/imprint/src/lib)
add_subdirectory(deps/piot/monotonic-time-c/src/lib)
add_subdirectory(deps/piot/secure-random-c/src/lib)
add_subdirectory(deps/piot/stats-c/src/lib)
add_subdirectory(deps/piot/tiny-libc/src/lib)


add_subdirectory(src)
