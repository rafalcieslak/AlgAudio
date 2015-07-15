include(ExternalProject)

ExternalProject_Add(
  liblo
  PREFIX liblo
  URL http://downloads.sourceforge.net/liblo/liblo-0.28.tar.gz
  CONFIGURE_COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/liblo/src/liblo/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/install
  BUILD_COMMAND ${MAKE}
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install
)
ExternalProject_Get_Property(liblo INSTALL_DIR)
set(LIBLO_INCLUDE_DIR "${INSTALL_DIR}/include" PARENT_SCOPE)

if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
  set(LIBLO_LIBRARY "${CMAKE_BINARY_DIR}/liblo-7.dll" PARENT_SCOPE)
  add_custom_command(
  	TARGET liblo POST_BUILD
  	COMMAND ${CMAKE_COMMAND} -E copy "${INSTALL_DIR}/bin/liblo-7.dll" "${CMAKE_BINARY_DIR}/"
  )
else()
  set(LIBLO_LIBRARY "${CMAKE_BINARY_DIR}/liblo.so.7" PARENT_SCOPE)
  add_custom_command(
  	TARGET liblo POST_BUILD
  	COMMAND ${CMAKE_COMMAND} -E copy "${INSTALL_DIR}/lib/liblo.so.7.2.0" "${CMAKE_BINARY_DIR}/liblo.so.7"
    COMMAND ln -sf "${CMAKE_BINARY_DIR}/liblo.so.7" "${CMAKE_BINARY_DIR}/liblo.so"
  )
endif()
