add_library(asm_lib "")

target_sources(
    asm_lib
    PRIVATE
        assembler.cc
		instruction.cc
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/assembler.h
		${CMAKE_CURRENT_LIST_DIR}/instruction.h
		${CMAKE_CURRENT_LIST_DIR}/instructions.h
		
    )

target_include_directories(
    asm_lib
    INTERFACE
        ..
    )
