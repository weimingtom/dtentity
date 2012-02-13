# - Find CEGUI
#   Find the CEGUI includes and library
#
#  CEGUI_INCLUDE_DIR - where to find CEGUI includes.
#  CEGUI_LIBRARIES   - List of libraries when CEGUI
#  CEGUI_FOUND       - True if CEGUI found.

IF(CEGUI_INCLUDE_DIR AND CEGUI_LIBRARIES AND CEGUI_OPENGLRENDERER_LIBRARIES)

  # in cache already
  SET(CEGUI_FOUND TRUE)

ELSE(CEGUI_INCLUDE_DIR AND CEGUI_LIBRARIES AND CEGUI_OPENGLRENDERER_LIBRARIES)


  INCLUDE(FindPackageHandleStandardArgs)
  INCLUDE(FindLibraryWithDebug)
  set(CEGUI_FIND_PATHS $ENV{VIEWER_EXT_DEP}/include/CEGUI /usr/include/CEGUI)
  FIND_PATH(CEGUI_INCLUDE_DIR CEGUI.h PATHS ${CEGUI_FIND_PATHS})
  FIND_LIBRARY_WITH_DEBUG(CEGUI_LIBRARIES CEGUIBase $ENV{VIEWER_EXT_DEP}/lib)
  FIND_LIBRARY_WITH_DEBUG(CEGUI_OPENGLRENDERER_LIBRARIES CEGUIOpenGLRenderer $ENV{VIEWER_EXT_DEP}/lib)

ENDIF(CEGUI_INCLUDE_DIR AND CEGUI_LIBRARIES AND CEGUI_OPENGLRENDERER_LIBRARIES)

