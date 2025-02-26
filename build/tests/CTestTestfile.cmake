# CMake generated Testfile for 
# Source directory: /usr/local/cmd/tests
# Build directory: /usr/local/cmd/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_cmd "/usr/local/cmd/build/bin/test_cmd")
set_tests_properties(test_cmd PROPERTIES  _BACKTRACE_TRIPLES "/usr/local/cmd/tests/CMakeLists.txt;5;add_test;/usr/local/cmd/tests/CMakeLists.txt;10;add_cmd_test;/usr/local/cmd/tests/CMakeLists.txt;0;")
add_test(test_command "/usr/local/cmd/build/bin/test_command")
set_tests_properties(test_command PROPERTIES  _BACKTRACE_TRIPLES "/usr/local/cmd/tests/CMakeLists.txt;5;add_test;/usr/local/cmd/tests/CMakeLists.txt;11;add_cmd_test;/usr/local/cmd/tests/CMakeLists.txt;0;")
add_test(test_ethernetframe "/usr/local/cmd/build/bin/test_ethernetframe")
set_tests_properties(test_ethernetframe PROPERTIES  _BACKTRACE_TRIPLES "/usr/local/cmd/tests/CMakeLists.txt;5;add_test;/usr/local/cmd/tests/CMakeLists.txt;12;add_cmd_test;/usr/local/cmd/tests/CMakeLists.txt;0;")
