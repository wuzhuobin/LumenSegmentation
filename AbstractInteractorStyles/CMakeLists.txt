project(InteractorStyles)

#VTK
find_package(VTK REQUIRED)
if (NOT VTK_FOUND)
	message(FATAL_ERROR "error: VTK is not found")
endif()
include(${VTK_USE_FILE})


#link other libraries
link_directories(
	${PROJECT_SOURCE_DIR}/lib
	)

include_directories(
	${MAIN_INCLUDE_DIRS}
)
SET(
	LIBRARY_OUTPUT_PATH
	${CMAKE_SOURCE_DIR}/lib
	${EXECUTABLE_OUTPUT_PATH}
)
file(
	GLOB 
	ABSTRACT_INTERACTORS_SRC 
	${PROJECT_SOURCE_DIR}/Abstract*.cxx
	PARENT_SCOPE
	)
file(
	GLOB 
	ABSTRACT_INTERACTORS_H 
	${PROJECT_SOURCE_DIR}/Abstract*.h
	PARENT_SCOPE
	)
file(
	GLOB 
	INTERACTORS_SRC  
	${PROJECT_SOURCE_DIR}/InteractorStyle*.cxx 
	PARENT_SCOPE
	)
file(
	GLOB 
	INTERACTORS_INC  
	${PROJECT_SOURCE_DIR}/InteractorStyle*.h 
	${PROJECT_SOURCE_DIR}/StyleSwitchMacro.h 
	PARENT_SCOPE
	)
add_library(
	InteractorStyles
	${ABSTRACT_INTERACTORS_SRC}
	${ABSTRACT_INTERACTORS_H}
	${INTERACTORS_INC}
	${INTERACTORS_SRC}
)

target_link_libraries(
	InteractorStyles
	${VTK_LIBRARIES}
)

source_group(
	"Header Files" 
	FILES 
	${ABSTRACT_INTERACTORS_H}
	${INTERACTORS_INC}
	)
source_group(
	"Source Files"
	FILES
	${ABSTRACT_INTERACTORS_SRC}
	${INTERACTORS_SRC}
	)