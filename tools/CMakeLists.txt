
include_directories(${pinocchIO_SOURCE_DIR}/library ${HDF5_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/library/pinocchIO})

add_executable(piols piols.c)
set(piols_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(piols pinocchIO)
install(TARGETS piols RUNTIME DESTINATION bin)

add_executable(piodump piodump.c)
set(piodump_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(piodump pinocchIO)
install(TARGETS piodump RUNTIME DESTINATION bin)


add_executable(pioversion pioversion.c)
set(pioversion_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(pioversion pinocchIO)
install(TARGETS pioversion RUNTIME DESTINATION bin)
