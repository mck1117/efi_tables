
add_library(lib STATIC)

target_sources(lib
    PRIVATE
        src/stub.cpp

        include/efi/interpolate.h
        include/efi/function.h
        include/efi/ratio_helper.h
        include/efi/table_bin.h
        include/efi/table_bin_array.h
        include/efi/table.h
        include/efi/table2d.h
)

target_include_directories(lib
    PUBLIC
        include
)
