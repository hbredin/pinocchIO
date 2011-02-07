file(GLOB pinocchIO_SOURCES *.c)
file(GLOB pinocchIO_HEADERS pinocchIO/*.h)
set(pinocchIO_PUBLICHEADERS pinocchIO/pinocchIO.h pinocchIO/pIOAttributes.h pinocchIO/pIODataset.h pinocchIO/pIODatatype.h pinocchIO/pIOFile.h pinocchIO/pIORead.h pinocchIO/pIOTimeComparison.h pinocchIO/pIOTimeline.h pinocchIO/pIOTypes.h pinocchIO/pIOWrite.h)

set(pinocchIO_INCLUDE_DIRS ${HDF5_INCLUDE_DIR} pinocchIO)
set(pinocchIO_LIBS ${HDF5_LIBRARY} ${HDF5_HL_LIBRARY})
include_directories(${pinocchIO_INCLUDE_DIRS})

add_library (pinocchIO SHARED ${pinocchIO_SOURCES} ${pinocchIO_HEADERS})

set_target_properties(pinocchIO PROPERTIES VERSION ${PINOCCHIO_VERSION} SOVERSION ${PINOCCHIO_API_VERSION})
set_target_properties(pinocchIO PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(pinocchIO ${pinocchIO_LIBS})

install(TARGETS pinocchIO DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${pinocchIO_PUBLICHEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDE}/pinocchIO)

