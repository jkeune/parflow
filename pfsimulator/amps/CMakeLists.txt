
file(GLOB_RECURSE COMMON_SRC_FILES common/*.c)

# Sets AMPS_SRC_FILES for AMPS transport layer being used in build
include("${PARFLOW_AMPS_LAYER}/CMakeLists.cmake")
string(REGEX REPLACE "([^;]+)" "${PARFLOW_AMPS_LAYER}/\\1" AMPS_SRC_FILES "${AMPS_SRC_FILES}")

add_library(amps STATIC ${COMMON_SRC_FILES} ${AMPS_SRC_FILES})

if (${PARFLOW_HAVE_MPI})
  target_include_directories (amps PUBLIC "${MPI_C_INCLUDE_PATH}")
endif (${PARFLOW_HAVE_MPI})

install(TARGETS amps DESTINATION lib)

# Install the AMPS transport layer specific scripts
set(AMPS_SCRIPTS batchmc bootmc freemc getmc killmc peekmc run)
string(REGEX REPLACE "([^;]+)" "${PARFLOW_AMPS_LAYER}/\\1" AMPS_SCRIPTS "${AMPS_SCRIPTS}")
install(FILES ${AMPS_SCRIPTS} DESTINATION bin)

