add_executable(cobs_test cobs_test.c)
target_link_libraries(cobs_test PRIVATE omuraisu_cobs)
target_include_directories(cobs_test PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_test(NAME cobs_test COMMAND cobs_test)

add_executable(cobs_cpp_test cobs_cpp_test.cpp)
target_link_libraries(cobs_cpp_test PRIVATE omuraisu_cpp_cobs)

add_test(NAME cobs_cpp_test COMMAND cobs_cpp_test)

add_executable(can_cpp_test can_cpp_test.cpp)
target_link_libraries(can_cpp_test PRIVATE omuraisu_can omuraisu_cpp_can)

add_test(NAME can_cpp_test COMMAND can_cpp_test)

add_executable(coordinate_cpp_test coordinate_cpp_test.cpp)
target_link_libraries(coordinate_cpp_test PRIVATE omuraisu_coordinate omuraisu_cpp_coordinate)

add_test(NAME coordinate_cpp_test COMMAND coordinate_cpp_test)

add_executable(pid_cpp_test pid_cpp_test.cpp)
target_link_libraries(pid_cpp_test PRIVATE omuraisu_pid omuraisu_cpp_pid)

add_test(NAME pid_cpp_test COMMAND pid_cpp_test)

add_executable(chassis_cpp_test chassis_cpp_test.cpp)
target_link_libraries(chassis_cpp_test PRIVATE
	omuraisu_coordinate
	omuraisu_chassis
	omuraisu_cpp_coordinate
	omuraisu_cpp_chassis
)

add_test(NAME chassis_cpp_test COMMAND chassis_cpp_test)

add_executable(dji_cpp_test dji_cpp_test.cpp)
target_link_libraries(dji_cpp_test PRIVATE
  omuraisu_dji
  omuraisu_cpp_dji
)

add_test(NAME dji_cpp_test COMMAND dji_cpp_test)

add_executable(servo_cpp_test servo_cpp_test.cpp)
target_link_libraries(servo_cpp_test PRIVATE
	omuraisu_servo
	omuraisu_cpp_servo
)

add_test(NAME servo_cpp_test COMMAND servo_cpp_test)

add_executable(controller_cpp_test controller_cpp_test.cpp)
target_link_libraries(controller_cpp_test PRIVATE
  omuraisu_controller
  omuraisu_cpp_controller
)

add_test(NAME controller_cpp_test COMMAND controller_cpp_test)
