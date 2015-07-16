include(ExternalProject)

# Liblo has a nasty minot bug, that is very unlikely to be fixed soon. It causes
# applications which include lo_cpp to have problems with multiple definitions
# imported from that header file. Therefore, instead of using the system
# build-in lo, we'll patch it and compile on our own (on MSYS builds this would
# be necessary anyway, as there is no bundled version). Because all
# modifications we do *never* actually get into what we distribute (these are
# only compilation fixes), we don't need to publish the source changes used.
# However, the diff can be found in file liblo-0.28.patch.
ExternalProject_Add(
  liblo
  PREFIX liblo
  URL http://downloads.sourceforge.net/liblo/liblo-0.28.tar.gz
  PATCH_COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/liblo-0.28.patch
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
