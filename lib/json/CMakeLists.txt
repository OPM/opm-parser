project(opm-json CXX)

add_library(opmjson JsonObject.cpp $<TARGET_OBJECTS:cjson>)
target_link_libraries(opmjson PUBLIC boost_filesystem)

if (CJSON_LIBRARY)
    target_link_libraries(opmjson PUBLIC cjson)
endif ()

target_include_directories(opmjson
    PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    PRIVATE $<TARGET_PROPERTY:cjson,INTERFACE_INCLUDE_DIRECTORIES>
)
set_target_properties(opmjson PROPERTIES
    VERSION   ${opm-parser_VERSION_MAJOR}.${opm-parser_VERSION_MINOR}
    SOVERSION ${opm-parser_VERSION_MAJOR}
)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS opmjson
        EXPORT  opm-parser-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(runJsonTests tests/jsonTests.cpp)
target_link_libraries(runJsonTests opmjson boost_test)
add_test(NAME runJsonTests
         COMMAND runJsonTests ${CMAKE_CURRENT_SOURCE_DIR}/tests/example1.json)
