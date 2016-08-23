Project(LumenSegmentation)
#VTK
find_package(VTK REQUIRED)
if (NOT VTK_FOUND)
	message(FATAL_ERROR "error: VTK is not found")
endif()
include(${VTK_USE_FILE})

SET(
	LIBRARY_OUTPUT_PATH
	${CMAKE_SOURCE_DIR}/lib
	${EXECUTABLE_OUTPUT_PATH}
)
SET(
	LUMENSEGMENTATION_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/LumenSegmentation.cxx
	)
SET(
	LUMENSEGMENTATION_INC
	${CMAKE_CURRENT_SOURCE_DIR}/LumenSegmentation.h
	)

add_library(
	LumenSegmentation
	${LUMENSEGMENTATION_SRC}
	${LUMENSEGMENTATION_INC}
	)

target_link_libraries(
	LumenSegmentation
	${VTK_LIBRARIES}
)

add_executable(
	LumenSegmentation
	${LUMENSEGMENTATION_SRC}
	${LUMENSEGMENTATION_INC}
	main.cpp
	MyImageViewer2.cxx
	MyImageViewer2.h
	)

target_link_libraries(
	LumenSegmentation
	${VTK_LIBRARIES}
)

source_group(
	"Header Files" 
	FILES 
	${LUMENSEGMENTATION_INC}
	)
source_group(
	"Source Files"
	FILES
	${LUMENSEGMENTATION_SRC}
	)