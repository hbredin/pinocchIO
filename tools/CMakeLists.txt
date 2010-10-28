

include_directories(${pinocchIO_SOURCE_DIR}/library)
link_directories(${CMAKE_BINARY_DIR}/library/pinocchIO})

add_executable(piols piols.c)
set(piols_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(piols pinocchIO)
install(TARGETS piols RUNTIME DESTINATION bin)

add_executable(piodump piodump.c)
set(piodump_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(piodump pinocchIO)
install(TARGETS piodump RUNTIME DESTINATION bin)

