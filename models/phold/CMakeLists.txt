INCLUDE_DIRECTORIES(${ROSS_BINARY_DIR})
IF(BGPM)
	INCLUDE_DIRECTORIES(${ROSS_SOURCE_DIR} ${BGPM_LIB})
ELSE(NOT(BGPM))
	INCLUDE_DIRECTORIES(${ROSS_SOURCE_DIR})
ENDIF(BGPM)

SET(phold_srcs
phold.c		phold.h)

ADD_EXECUTABLE(phold ${phold_srcs})
ADD_EXECUTABLE(phold_test ${phold_srcs})

IF(BGPM)
	TARGET_LINK_LIBRARIES(phold ROSS imp_bgpm m)
	TARGET_LINK_LIBRARIES(phold_test ROSS imp_bgpm m)
ELSE(NOT(BGPM))
	TARGET_LINK_LIBRARIES(phold ROSS m)
	TARGET_LINK_LIBRARIES(phold_test ROSS m)
ENDIF(BGPM)

ROSS_TEST_SCHEDULERS(phold)
ROSS_TEST_INSTRUMENTATION(phold)

SET_TARGET_PROPERTIES(phold_test PROPERTIES COMPILE_DEFINITIONS TEST_COMM_ROSS)
ROSS_TEST_SCHEDULERS(phold_test)
ROSS_TEST_INSTRUMENTATION(phold_test)
