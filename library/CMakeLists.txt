file(GLOB pinocchIO_SOURCES *.c)
file(GLOB pinocchIO_HEADERS *.h)
set(pinocchIO_PUBLICHEADERS pIOAttributes.h pIODataset.h pIODatatype.h pIOFile.h pIOServer.h pIORead.h pIOTimeComparison.h pIOTimeline.h pIOTypes.h pIOWrite.h pinocchIO.h)

set(pinocchIO_INCLUDE_DIRS ${HDF5_INCLUDE_DIR} ${LIBCONFIG_INCLUDE_DIR})
set(pinocchIO_LIBS ${HDF5_LIBRARY} ${HDF5_HL_LIBRARY} ${LIBCONFIG_LIBRARY})
include_directories(${pinocchIO_INCLUDE_DIRS})

add_library (pinocchIO SHARED ${pinocchIO_SOURCES} ${pinocchIO_HEADERS})

set_target_properties(pinocchIO PROPERTIES VERSION ${PINOCCHIO_VERSION} SOVERSION ${PINOCCHIO_API_VERSION})
set_target_properties(pinocchIO PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(pinocchIO ${pinocchIO_LIBS})

install(TARGETS pinocchIO DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${pinocchIO_PUBLICHEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDE}/pinocchIO)

