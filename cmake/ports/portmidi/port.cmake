include_guard(GLOBAL)

if(WIN32)
  set(lib lib/portmidi.lib)
else()
  set(lib lib/libportmidi.a)
endif()

declare_port(
  "github:PortMidi/portmidi#master"
  portmidi
  ARGS
    -DBUILD_SHARED_LIBS=OFF
  BYPRODUCTS
    ${lib}
)

add_library(portmidi STATIC IMPORTED GLOBAL)

add_dependencies(portmidi ${portmidi})

set_target_properties(
  portmidi
  PROPERTIES
  IMPORTED_LOCATION "${portmidi_PREFIX}/${lib}"
)

file(MAKE_DIRECTORY "${portmidi_PREFIX}/include")

target_include_directories(
  portmidi
  INTERFACE "${portmidi_PREFIX}/include"
)
