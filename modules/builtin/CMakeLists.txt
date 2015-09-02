
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/modules/builtin.xml"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/builtin.xml" "${CMAKE_BINARY_DIR}/modules"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/builtin.xml"
)

add_custom_target(copy_builtin_xml DEPENDS "${CMAKE_BINARY_DIR}/modules/builtin.xml")
add_custom_target(modules_builtin ALL)
add_dependencies(modules_builtin copy_builtin_xml)
