add_library(rp2040_gfxlib STATIC 
        fixed_point.cpp
    )

target_include_directories(rp2040_gfxlib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(rp2040_gfxlib PRIVATE cxx_std_17)