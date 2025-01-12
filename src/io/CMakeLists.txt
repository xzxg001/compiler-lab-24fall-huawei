set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/local" CACHE PATH "Default install path" FORCE)
add_library(cminus_io io.c)

install(
    TARGETS cminus_io
    ARCHIVE DESTINATION lib
)
