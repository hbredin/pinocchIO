file(GLOB gepetto_SOURCES *.c)
file(GLOB gepetto_HEADERS *.h)
set(gepetto_PUBLICHEADERS gepetto.h gptTypes.h gptServer.h gptConfig.h)

set(gepetto_INCLUDE_DIRS ${LIBCONFIG_INCLUDE_DIR})
set(gepetto_LIBS ${LIBCONFIG_LIBRARY} pinocchIO)
include_directories(${gepetto_INCLUDE_DIRS})

add_library (gepetto SHARED ${gepetto_SOURCES} ${gepetto_HEADERS})

set_target_properties(gepetto PROPERTIES VERSION ${PINOCCHIO_VERSION} SOVERSION ${PINOCCHIO_API_VERSION})
set_target_properties(gepetto PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(gepetto ${gepetto_LIBS})



install(TARGETS gepetto DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${gepetto_PUBLICHEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDE}/gepetto)

