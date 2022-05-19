
# install_vcpkg(<vcpkg commit, branch, or tag>)
# Installs vcpkg into the ${VCPKG_ROOT} directory.
function(install_vcpkg vcpkgCommit)
    if(NOT EXISTS ${VCPKG_ROOT})
        message(STATUS "Cloning vcpkg in ${VCPKG_ROOT}")
        execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT})
    endif()

    message(STATUS "Checking out vcpkg commit ${vcpkgCommit}")
    execute_process(COMMAND git checkout ${vcpkgCommit} WORKING_DIRECTORY ${VCPKG_ROOT})

    if(NOT EXISTS ${VCPKG_ROOT}/README.md)
    message(FATAL_ERROR "***** FATAL ERROR: Could not clone vcpkg *****")
    endif()

    if(WIN32)
    set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
    set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)
    else()
    set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
    set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.sh)
    endif()

    if(NOT EXISTS ${VCPKG_EXEC})
        message(STATUS "Bootstrapping vcpkg in ${VCPKG_ROOT}")
        execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${VCPKG_ROOT})
    endif()

    if(NOT EXISTS ${VCPKG_EXEC})
        message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
    endif()
endfunction()

# set_vcpkg_dependencies(<package name>...)
# Adds a list of dependencies to download, build and install in the vcpkg instance located at ${VCPKG_ROOT}.
function(set_vcpkg_dependencies)
    set(VCPKG_DEPENDENCIES ${ARGN})
    set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
    message(STATUS "Checking dependencies in ${VCPKG_ROOT} for platform ${VCPKG_TARGET_TRIPLET}")
    execute_process(COMMAND ${VCPKG_EXEC} install ${VCPKG_DEPENDENCIES} --triplet ${VCPKG_TARGET_TRIPLET} WORKING_DIRECTORY ${VCPKG_ROOT})
endfunction()
