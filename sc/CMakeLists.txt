

set(SC_SCRIPTS main.sc)

add_custom_target(sc_scripts ALL)

FOREACH(SCFILE ${SC_SCRIPTS})
  add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/sc/${SCFILE}"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SCFILE}" "${CMAKE_BINARY_DIR}/sc"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${SCFILE}"
  )

  add_custom_target("copy_${SCFILE}" DEPENDS "${CMAKE_BINARY_DIR}/sc/${SCFILE}")
  add_dependencies(
    sc_scripts
    "copy_${SCFILE}"
  )
ENDFOREACH(SCFILE)
