set(MWO_SHARDSERVER_FILES
	mwo_shardserver.cpp
	mwo_shardserver.hpp
        mwo_shardtomasterconnection.cpp
        mwo_shardtomasterconnection.hpp
)

source_group(apps\\mwo_shardserver FILES ${MWO_SHARDSERVER_FILES})

add_executable(mwo_shardserver ${MWO_SHARDSERVER_FILES})

target_link_libraries(mwo_shardserver
	components
)

if (BUILD_WITH_CODE_COVERAGE)
	add_definitions(--coverage)
	target_link_libraries(mwo_shardserver gcov)
endif()
