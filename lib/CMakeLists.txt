#BUG include contrib/CMake/toplevel.txt

# read variable from file.
# (c) 2020 Felix Salfelder
#     GPLv3
macro( readVariable FILENAME VARIABLE OUTVAR_NAME)
    # not sure about the scoping
    unset(FILE_BUFFER)
    unset(_RESULT)
	 file( READ ${FILENAME} FILE_BUFFER )

    # foreach does not work with newlines.
    string(REGEX REPLACE "\n" "${Esc};" BUFFER "${FILE_BUFFER}")

    foreach(LINE ${BUFFER})
       if(CONT)
           set(CONT False)
           if("${LINE}" MATCHES "\\\\${Esc}$")
               set(CONT True)
           endif()
           list( APPEND _RESULT ${LINE})
           string(REGEX REPLACE "\\\\" "" LINE ${LINE})
       endif()
       if("${LINE}" MATCHES "^${VARIABLE}")
           string(REPLACE "${VARIABLE} =" "" _RESULT ${LINE})
           if("${LINE}" MATCHES "\\\\${Esc}$")
               set(CONT True)
           endif()
           string(REGEX REPLACE "\\\\" "" LINE ${LINE})
       endif()
    endforeach()

    # turn whitespace into ;
    separate_arguments(_RESULT)

    # seems to be global
    set(${OUTVAR_NAME} ${_RESULT})
endmacro()

# Avoid build system maintenance madness. Use one list for all of them
readVariable("Make1" "RAW_SRCS" "RAW_SRCS")

# message(STATUS "===========TEST===============")
# message(STATUS "RAW ${RAW_SRCS}")
# message(STATUS "===========END TEST===============")


add_library(gnucap SHARED
	${RAW_SRCS}
)

target_link_libraries(gnucap ${DL_LIBRARIES} ${READLINE_LIBS})

set_target_properties(gnucap PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_FULL_LIBDIR})

install(TARGETS gnucap DESTINATION ${CMAKE_INSTALL_LIBDIR})
