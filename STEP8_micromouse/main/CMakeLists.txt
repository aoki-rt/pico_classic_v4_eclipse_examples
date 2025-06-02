# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.cpp device.c run.cpp sensor.cpp adjust.cpp fast.cpp map_manager.cpp misc.cpp search.cpp flash.cpp TMC5240.cpp       # list the source files of this component
    INCLUDE_DIRS  "."       # optional, add here public include directories
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES 	        # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
