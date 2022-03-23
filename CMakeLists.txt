cmake_minimum_required(VERSION 3.16)

project(helloworld VERSION 1.0.0 LANGUAGES CXX)

set(PROGRAM_NAME helloworld)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/CONAN")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES ${PROGRAM_NAME}_en_GB.ts ${PROGRAM_NAME}_en_US.ts ${PROGRAM_NAME}_nb_NO.ts)

set(SOURCE_FILES main.cpp)

add_executable(${PROGRAM_NAME}
    ${SOURCE_FILES}
    ${TS_FILES}
)

set_target_properties(${PROGRAM_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER ${PROGRAM_NAME}.test.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt6_add_translations(${PROGRAM_NAME} TS_FILES ${TS_FILES})
qt6_create_translation(QM_FILES ${SOURCE_FILES} ${TS_FILES})

target_link_libraries(${PROGRAM_NAME} PRIVATE Qt6::Widgets)

set_target_properties(${PROGRAM_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER ${PROGRAM_NAME}.test.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if( APPLE )
    # Set targets
    set(SOURCE_RESOURCE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/Resources) 
    set_target_properties(${PROGRAM_NAME} PROPERTIES RESOURCE "${APP_RESOURCES}")
    set_target_properties(${PROGRAM_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${SOURCE_RESOURCE_FOLDER}/Info.plist.in" )
ENDIF( APPLE)


##### CPACK
# Set bundle information
set(CPACK_BUNDLE_NAME "${PROGRAM_NAME}")
set(CPACK_PACKAGE_NAME "${PROGRAM_NAME}")
set(CPACK_PACKAGE_VENDOR "CMake.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Test application.")
set(CPACK_PACKAGE_VERSION "${PROGRAM_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROGRAM_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROGRAM_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROGRAM_PATCH_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROGRAM_NAME}")

configure_file("README" "README.txt" COPYONLY)
configure_file("LICENSE.in" "LICENSE.txt" NEWLINE_STYLE CRLF)

set(CPACK_RESOURCE_FILE_README "${PROJECT_BINARY_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CPACK_RESOURCE_FILE_README}")

# Install readme file
install(FILES "${PROJECT_BINARY_DIR}/README.txt" DESTINATION . COMPONENT Resources)
install(FILES "${PROJECT_BINARY_DIR}/LICENSE.txt" DESTINATION . COMPONENT Resources)

# Install stuff
# Copied from: http://stackoverflow.com/questions/14291665/dylibs-and-frameworks-in-bundle-app
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set( plugin_dest_dir bin )
set( qtconf_dest_dir bin )
set( APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROGRAM_NAME}" )

if( APPLE )
    set( plugin_dest_dir ${PROGRAM_NAME}.app/Contents/ )
    set( qtconf_dest_dir ${PROGRAM_NAME}.app/Contents/Resources )
    set( APPS "\${CMAKE_INSTALL_PREFIX}/${PROGRAM_NAME}.app" )
endif( APPLE )

if( WIN32 )
    include(InstallRequiredSystemLibraries)

    # Create start menu icons
    set(CPACK_NSIS_CREATE_ICONS "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROGRAM_NAME}.lnk' '\$INSTDIR\\\\bin\\\\${PROGRAM_NAME}.exe'")
    set(CPACK_NSIS_DELETE_ICONS "Delete '\$SMPROGRAMS\\\\$MUI_TEMP\\\\${PROGRAM_NAME}.lnk'")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
  set( APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROGRAM_NAME}.exe" )
endif( WIN32 )

#--------------------------------------------------------------------------------
# Install the application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
install( TARGETS ${PROGRAM_NAME}
    BUNDLE DESTINATION . COMPONENT Runtime
    #RESOURCE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
)


message("QT_PLUGIN_DIR: ${QT_PLUGIN_DIR}")
#--------------------------------------------------------------------------------
# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
install( DIRECTORY "${QT_PLUGIN_DIR}/imageformats"
    DESTINATION ${plugin_dest_dir}/PlugIns
    COMPONENT Runtime
)

install( DIRECTORY "${QT_PLUGIN_DIR}/platforms"
    DESTINATION ${plugin_dest_dir}/PlugIns
    COMPONENT Runtime
)

install( DIRECTORY "${QT_PLUGIN_DIR}/styles"
    DESTINATION ${plugin_dest_dir}/PlugIns
    COMPONENT Runtime
)

#--------------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
install( CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime
)

#--------------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.
# directories to look for dependencies
# Conan installs the shared libraries in ${CMAKE_BINARY_DIR}/lib, so add it to DIRS
set( DIRS ${QT_LIBRARY_DIRS} ${${PROGRAM_NAME}_LIBRARIES} ${ALL_INCLUDE_DIRECTORIES} ${CMAKE_BINARY_DIR}/conan/lib)

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
install( CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    set(BU_CHMOD_BUNDLE_ITEMS ON)
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime
)

# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
if(WIN32)
  set(CPACK_GENERATOR NSIS)
elseif(APPLE)
  set(CPACK_GENERATOR DragNDrop)
else()
  set(CPACK_GENERATOR TGZ)
endif()

include( CPack )

