#### Fast .stl viewer #####
# Copyright 2016
# Original Project Author: Matt Keeter 2014
# Author: Paul Tsouchlos

cmake_minimum_required(VERSION 3.3)

# Set the version number
set (FSTL_VERSION_MAJOR "0")
set (FSTL_VERSION_MINOR "9")
set (FSTL_VERSION_PATCH "3")
set (FSTL_VERSION "${FSTL_VERSION_MAJOR}.${FSTL_VERSION_MINOR}.${FSTL_VERSION_PATCH}")

# Add version definitions to use within the code. 
add_definitions(-DFSTL_VERSION="${FSTL_VERSION}")
project(fstl)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

#set project sources
set(Project_Sources src/app.cpp
src/backdrop.cpp
src/canvas.cpp
src/glmesh.cpp
src/loader.cpp
src/main.cpp
src/mesh.cpp
src/window.cpp)

#set project headers. 
set(Project_Headers src/app.h
src/backdrop.h
src/canvas.h
src/glmesh.h
src/loader.h
src/mesh.h
src/window.h)

#set project resources and icon resource
set(Project_Resources qt/qt.qrc gl/gl.qrc)
set(Icon_Resource exe/fstl.rc)

#find required packages. 
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5OpenGl REQUIRED)
find_package(OpenGL REQUIRED)
set(LIBRARIES ${LIBRARIES} ${OPENGL_LIBRARIES})

#add resources to RCC
qt5_add_resources(Project_Resources_RCC ${Project_Resources})

#include opengl files. 
include_directories(${QT_QTOPENGL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} )

add_executable(fstl WIN32 ${Project_Sources} ${Project_Headers} ${Project_Resources_RCC} ${Icon_Resource})
target_link_libraries(fstl Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL ${LIBRARIES})
set(Fstl_LINK_FLAGS ${CMAKE_CURRENT_SOURCE_DIR}/${Icon_Resource})
set_target_properties(fstl PROPERTIES LINK_FLAGS ${Fstl_LINK_FLAGS})

if(WIN32)

	set(QT_USE_QTMAIN true)
	
	if(MSVC)
		set_source_files_properties(fstl PROPERTIES LINKER_LANGUAGE "CXX")
		set_target_properties(fstl PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
		install(TARGETS fstl DESTINATION bin COMPONENT all)
		
		install(FILES
		${Qt5Core_DIR}/../../../bin/libEGL.dll
        ${Qt5Core_DIR}/../../../bin/libGLESv2.dll
        ${Qt5Core_DIR}/../../../bin/Qt5Core.dll
        ${Qt5Core_DIR}/../../../bin/Qt5Gui.dll
        ${Qt5Core_DIR}/../../../bin/Qt5Network.dll
        ${Qt5Core_DIR}/../../../bin/Qt5OpenGL.dll
		${Qt5Core_DIR}/../../../bin/Qt5Widgets.dll
		DESTINATION bin COMPONENT all)
		
		#install files in the imageformats directory. 
		install (FILES
		${Qt5Core_DIR}/../../../plugins/imageformats/qdds.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qgif.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qicns.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qico.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qjp2.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qjpeg.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qmng.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qsvg.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qtga.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qtiff.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qwbmp.dll
		${Qt5Core_DIR}/../../../plugins/imageformats/qwebp.dll
		DESTINATION bin/imageformats COMPONENT all)
		
		#install file in the platforms directory.
		install (FILES
		${Qt5Core_DIR}/../../../plugins/platforms/qwindows.dll
		DESTINATION bin/platforms COMPONENT all
		)
		
	endif(MSVC)
endif(WIN32)

#installer stuff
	SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fast .stl file viewer.")
	SET(CPACK_PACKAGE_VERSION_MAJOR ${FSTL_VERSION_MAJOR})
	SET(CPACK_PACKAGE_VERSION_MINOR ${FSTL_VERSION_MINOR})
	SET(CPACK_PACKAGE_VERSION_PATCH ${FSTL_VERSION_PATCH})
	SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
	SET(CPACK_NSIS_MODIFY_PATH ON)
	SET(CPACK_NSIS_MUI_FINISHPAGE_RUN ON)
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY "fstl-${FSTL_VERSION}")
	set(MY_INCLUDE_DIR ${CPACK_PACKAGE_INSTALL_DIRECTORY}/bin)
	set(MY_LIB_DIR ${CPACK_PACKAGE_INSTALL_DIRECTORY}/dlls)
	SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\fstl.exe")
	SET(CPACK_NSIS_DISPLAY_NAME "fstl ${FSTL_VERSION}")
	SET(CPACK_NSIS_MODIFY_PATH ON)
	set(CPACK_NSIS_CREATE_ICONS_EXTRA
"CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\fstl.lnk' '$INSTDIR\\\\bin\\\\fstl.exe'")
	SET(CPACK_COMPONENTS_ALL all)
	
if(MSVC)
	#custom commands based on: https://gist.github.com/Rod-Persky/e6b93e9ee31f9516261b
	add_custom_command(TARGET fstl POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libEGLd.dll          ${CMAKE_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libGLESv2d.dll       ${CMAKE_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::OpenGL>        $<TARGET_FILE_DIR:${PROJECT_NAME}>

		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libEGL.dll          ${CMAKE_BINARY_DIR}/Release
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/libGLESv2.dll       ${CMAKE_BINARY_DIR}/Release
	)
endif(MSVC)