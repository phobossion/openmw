set(MWO_MASTERSERVER_FILES
	mwo_masterserver.cpp
	mwo_masterserver.hpp
        mwo_clientconnection.hpp
        mwo_clientconnection.cpp
)

source_group(apps\\mwo_masterserver FILES ${MWO_MASTERSERVER_FILES})

add_executable(mwo_masterserver ${MWO_MASTERSERVER_FILES})

target_link_libraries(mwo_masterserver
	components
)

if (BUILD_WITH_CODE_COVERAGE)
	add_definitions(--coverage)
	target_link_libraries(mwo_masterserver gcov)
endif()
