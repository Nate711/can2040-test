add_library(pupperv3 mg4005.cc)
target_include_directories(pupperv3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(pupperv3 PUBLIC can2040)
