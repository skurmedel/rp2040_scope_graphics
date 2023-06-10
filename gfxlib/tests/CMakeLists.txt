include(FetchContent)

FetchContent_Declare(
    catch
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.3.2
)

FetchContent_MakeAvailable(catch)

add_executable(rp2040_gfxlib_tests 
        test_fixed_point.cpp
    )

target_compile_features(rp2040_gfxlib_tests PRIVATE cxx_std_17)
target_link_libraries(rp2040_gfxlib_tests PRIVATE Catch2WithMain rp2040_gfxlib)