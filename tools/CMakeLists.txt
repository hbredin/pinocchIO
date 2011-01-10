
set (cli_INCLUDE_DIRS ${pinocchIO_SOURCE_DIR}/library/pio ${pinocchIO_SOURCE_DIR}/library/gpt ${HDF5_INCLUDE_DIR})

if (LIBCONFIG_FOUND)
   set (cli_INCLUDE_DIRS ${cli_INCLUDE_DIRS} ${LIBCONFIG_INCLUDE_DIR})
endif (LIBCONFIG_FOUND)

include_directories(${cli_INCLUDE_DIRS})

add_executable(pioinit pioinit.c)
target_link_libraries(pioinit pinocchIO)
install(TARGETS pioinit RUNTIME DESTINATION bin)

add_executable(piols piols.c)
target_link_libraries(piols pinocchIO)
install(TARGETS piols RUNTIME DESTINATION bin)

add_executable(piodump piodump.c)
target_link_libraries(piodump pinocchIO)
install(TARGETS piodump RUNTIME DESTINATION bin)

add_executable(piorm piorm.c)
target_link_libraries(piorm pinocchIO)
install(TARGETS piorm RUNTIME DESTINATION bin)

add_executable(pioaggregate pioaggregate.c)
target_link_libraries(pioaggregate pinocchIO)
install(TARGETS pioaggregate RUNTIME DESTINATION bin)

add_executable(piocp piocp.c)
target_link_libraries(piocp pinocchIO)
install(TARGETS piocp RUNTIME DESTINATION bin)

add_executable(ascii2pio ascii2pio.c fgetln.c)
target_link_libraries(ascii2pio pinocchIO)
install(TARGETS ascii2pio RUNTIME DESTINATION bin)

add_executable(pioversion pioversion.c)
target_link_libraries(pioversion pinocchIO)
install(TARGETS pioversion RUNTIME DESTINATION bin)

if (LIBCONFIG_FOUND)
   add_executable(gptdump gptdump.c)
   target_link_libraries(gptdump gepetto)
   install(TARGETS gptdump RUNTIME DESTINATION bin)
endif (LIBCONFIG_FOUND)
