include(ExternalProject)

ExternalProject_Add(
  liblo
  URL http://downloads.sourceforge.net/liblo/liblo-0.28.tar.gz
  CONFIGURE_COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/liblo/src/liblo/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/install
  BUILD_COMMAND ${MAKE}
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install
)
ExternalProject_Get_Property(liblo INSTALL_DIR)
set(LIBLO_INCLUDE_DIR "${INSTALL_DIR}/include" PARENT_SCOPE)
set(LIBLO_LIBRARY "${CMAKE_BINARY_DIR}/liblo-7.dll" PARENT_SCOPE)

#TODO: Multi-OS
add_custom_command(
	TARGET liblo POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${INSTALL_DIR}/bin/liblo-7.dll" "${CMAKE_BINARY_DIR}/"
)
