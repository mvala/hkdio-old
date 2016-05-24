set(PACKAGE HkTasks)

set(SRCS 
HkTask.cxx
HkTaskInput.cxx
HkTaskRsn.cxx
HkAnalysisSelector.cxx
HkAnalysisSelectorIO.cxx
)
set (MY_INCLUDE_DIRS
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/HkBase
${ROOT_INCLUDE_DIR}
)

set (MY_LINK_DIRS
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_BINARY_DIR}/HkBase
${ROOT_LIBRARY_DIR}
)

include_directories( ${MY_INCLUDE_DIRS} )
link_directories( "${MY_LINK_DIRS}")

String(REPLACE ".cxx" ".h" HDRS "${SRCS}")
ROOT_GENERATE_DICTIONARY( G__${PACKAGE} ${HDRS} LINKDEF ${PACKAGE}LinkDef.h)
list(APPEND SRCS G__${PACKAGE}.cxx)

add_library (${PACKAGE} SHARED ${SRCS} ${HDRS})
target_link_libraries (${PACKAGE} ${ROOT_LIBRARIES})

install(TARGETS ${PACKAGE} LIBRARY DESTINATION lib64 COMPONENT RUNTIME)

add_par(${PACKAGE})
