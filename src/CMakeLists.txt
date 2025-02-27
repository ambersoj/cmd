# Create the cmd_lib library
add_library(cmd_lib STATIC Cmd.cpp Command.cpp EthernetFrame.cpp RxObserver.cpp DCE.cpp)
target_include_directories(cmd_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(cmd_lib PRIVATE pthread net pcap)
add_executable(cmd main.cpp)
target_include_directories(cmd PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(cmd PRIVATE cmd_lib)

## Place executables in bin/
set_target_properties(cmd PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
