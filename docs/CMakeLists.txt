# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

list(APPEND GENERATED_RST "")
#generate rst
macro(generate_rst in out)
    add_custom_command(OUTPUT ${out}
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/c2rst.py ${in}
                       PRE_BUILD
                       COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/c2rst.py ${in} ${out})
    list(APPEND GENERATED_RST "${out}")
endmacro()

generate_rst(${PROJECT_SOURCE_DIR}/tutorials/nodesets/nodeset.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tutorials/nodeset.rst)
generate_rst(${PROJECT_SOURCE_DIR}/tutorials/methods/methods.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tutorials/methods.rst)
generate_rst(${PROJECT_SOURCE_DIR}/tutorials/variables/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tutorials/variables.rst)

find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)

add_custom_target(Sphinx ALL
                  COMMAND
                  #${SPHINX_EXECUTABLE} -b html
                  /usr/local/bin/sphinx-build -v -b html
                  -Dbreathe_projects.modernopc=${CMAKE_CURRENT_BINARY_DIR}/doc_doxygen/xml/
                  ${SPHINX_SOURCE} ${SPHINX_BUILD}
                  DEPENDS ${GENERATED_RST}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation with Sphinx")
                
add_dependencies(Sphinx doc_doxygen)
#add_dependencies(Sphinx ${GENERATED_RST})