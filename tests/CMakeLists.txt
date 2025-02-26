# Function to add tests easily
function(add_cmd_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE cmd_lib)
    add_test(NAME ${name} COMMAND ${name})
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endfunction()

# Add test executables
add_cmd_test(test_cmd)
add_cmd_test(test_command)
add_cmd_test(test_ethernetframe)
