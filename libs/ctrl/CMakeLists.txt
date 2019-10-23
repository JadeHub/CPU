add_library(ctrl_lib "")

target_sources(
    ctrl_lib
    PRIVATE
        ctrl_eeprom.cc
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/constants.h
    )

target_include_directories(
    ctrl_lib
    INTERFACE
	..
	)
