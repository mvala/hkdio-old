set(PACKAGE HkZmq)

set (MY_INCLUDE_DIRS
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/HkBase
${ROOT_INCLUDE_DIR}
${LIBZMQ_INCLUDE_DIRS}
${CZMQ_INCLUDE_DIRS}
${ZYRE_INCLUDE_DIRS}
)

set (MY_LINK_DIRS
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_BINARY_DIR}/HkBase
${ROOT_LIBRARY_DIR}
)

include_directories( ${MY_INCLUDE_DIRS} )
link_directories( "${MY_LINK_DIRS}")

add_executable (hk-zmq hk-zmq.cxx)
target_link_libraries (hk-zmq ${LIBZMQ_LIBRARIES} ${CZMQ_LIBRARIES})

add_executable (hk-zyre hk-zyre.cxx)
target_link_libraries (hk-zyre ${LIBZMQ_LIBRARIES} ${CZMQ_LIBRARIES} ${ZYRE_LIBRARIES})

set(TARGETS hk-zmq)
install(TARGETS ${TARGETS} RUNTIME DESTINATION bin COMPONENT RUNTIME)