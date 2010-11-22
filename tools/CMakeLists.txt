
include_directories(${pinocchIO_SOURCE_DIR}/library/pio ${pinocchIO_SOURCE_DIR}/library/gpt ${HDF5_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/library/pinocchIO})

add_executable(piols piols.c)
set(piols_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(piols pinocchIO)
install(TARGETS piols RUNTIME DESTINATION bin)

add_executable(piodump piodump.c)
set(piodump_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(piodump pinocchIO)
install(TARGETS piodump RUNTIME DESTINATION bin)

add_executable(piorm piorm.c)
set(piorm_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(piorm pinocchIO)
install(TARGETS piorm RUNTIME DESTINATION bin)

add_executable(ascii2pio ascii2pio.c fgetln.c)
set(ascii2pio_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(ascii2pio pinocchIO)
install(TARGETS ascii2pio RUNTIME DESTINATION bin)

add_executable(pioversion pioversion.c)
set(pioversion_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(pioversion pinocchIO)
install(TARGETS pioversion RUNTIME DESTINATION bin)

add_executable(gptdump gptdump.c)
set(gptdump_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/library)
target_link_libraries(gptdump gepetto)
install(TARGETS gptdump RUNTIME DESTINATION bin)