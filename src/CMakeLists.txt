add_library(meshfiles STATIC polylla.hpp triangulation.hpp)
target_include_directories(meshfiles PUBLIC ${PROJECT_SOURCE_DIR}/src)