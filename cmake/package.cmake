macro(ADD_PACKAGE PACKAGE_NAME HOMEWORK_NUMBER)
    set(CPACK_GENERATOR "DEB")

    set(CPACK_PACKAGE_NAME ${PACKAGE_NAME})

    # which is useful in case of packing only selected components instead of the whole thing
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Otus C++ Professional course HW ${HOMEWORK_NUMBER}")
    set(CPACK_VERBATIM_VARIABLES YES)

    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})

    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

    set(CPACK_PACKAGE_VERSION_MAJOR 0)
    set(CPACK_PACKAGE_VERSION_MINOR 0)
    set(CPACK_PACKAGE_VERSION_PATCH ${BUILD_NUMBER})

    set(CPACK_PACKAGE_CONTACT "vitalyisaev2@gmail.com")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Vitaly Isaev")

    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/../README.md")

    include(CPack)
endmacro()