# Building app using HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(beatbox ${MY_SOURCES})

#Use the HAL library and ALSA support
find_package(ALSA REQUIRED)
target_link_libraries(beatbox LINK_PRIVATE hal asound pthread)

#create remote location

add_custom_command(TARGET beatbox POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
    "$<TARGET_FILE:beatbox>"
    "~/cmpt433/public/myApps/beatbox" 
    COMMENT "Copying ARM executable to public NFS directory"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
    "${CMAKE_SOURCE_DIR}/wave-files"
    "~/cmpt433/public/myApps/wave-files"
    COMMENT "Copying WAVE files to public NFS directory")