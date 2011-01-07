
add_subdirectory (pio)

if (LIBCONFIG_FOUND)
   add_subdirectory (gpt)
endif (LIBCONFIG_FOUND)
