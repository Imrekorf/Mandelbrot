find_package(Git)

# Git information

# The 'real' git information file
set(GITREV_BARE_FILE git_rev.h)
# The temporary git information file
set(GITREV_BARE_TMP git_rev_tmp.h)
set(GITREV_FILE ${CMAKE_SOURCE_DIR}/res/${GITREV_BARE_FILE})
set(GITREV_TMP ${CMAKE_SOURCE_DIR}/res/${GITREV_BARE_TMP})

add_custom_command(
  OUTPUT ${GITREV_FILE}
  COMMAND ${CMAKE_COMMAND} -E echo_append "#define GIT_HASH " >> ${GITREV_TMP}
  COMMAND ${GIT_EXECUTABLE} describe --always --dirty --abbrev=7 --match="NoTagWithThisName" >> ${GITREV_TMP}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GITREV_TMP} ${GITREV_FILE}
  COMMAND ${CMAKE_COMMAND} -E remove ${GITREV_TMP}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  BYPRODUCTS ${GITREV_FILE}
  VERBATIM
)

add_custom_target(GIT_HASH DEPENDS ${GITREV_FILE})