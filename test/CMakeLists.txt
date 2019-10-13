add_executable(
    unit_tests
    asm_test.cc
    )

target_link_libraries(
    unit_tests
    gtest_main
    asm_lib
    )

add_test(
  NAME
    unit
  COMMAND
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
  )
