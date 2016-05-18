set(PACKAGE HkBase)

set(SRCS 
HkTrack.cxx
HkEvent.cxx
HkAnalysisSelector.cxx
HkAnalysisSelectorIO.cxx
)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}")
link_directories( "${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${ROOT_INCLUDE_DIR})
link_directories( ${ROOT_LIBRARY_DIR} )

String(REPLACE ".cxx" ".h" HDRS "${SRCS}")
ROOT_GENERATE_DICTIONARY( G__${PACKAGE} ${HDRS} LINKDEF ${PACKAGE}LinkDef.h)
list(APPEND SRCS G__${PACKAGE}.cxx)

add_library (${PACKAGE} SHARED ${SRCS} ${HDRS})
target_link_libraries (${PACKAGE} ${ROOT_LIBRARIES})

install(TARGETS ${PACKAGE} LIBRARY DESTINATION lib64 COMPONENT RUNTIME)

add_par(${PACKAGE})
