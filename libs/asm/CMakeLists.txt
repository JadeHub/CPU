add_library(asm_lib "")

target_sources(
    asm_lib
    PRIVATE
        program.cc
		instruction.cc
		source_line.cc
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/program.h
		${CMAKE_CURRENT_LIST_DIR}/instruction.h
		${CMAKE_CURRENT_LIST_DIR}/instructions.h
		${CMAKE_CURRENT_LIST_DIR}/source_line.h
    )

target_link_libraries(
	asm_lib
	ctrl_lib
)

target_include_directories(
    asm_lib
    INTERFACE
        ..
    )
