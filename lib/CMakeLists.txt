
add_library(lib STATIC)

target_sources(lib
    PRIVATE
        src/stub.cpp
        src/sensor.cpp

        include/efi/interpolate.h
        include/efi/function.h
        include/efi/ratio_helper.h
        include/efi/table/table_bin.h
        include/efi/table/table_bin_array.h
        include/efi/table/table.h
        include/efi/table/table2d.h
        include/efi/table/table3d.h

        include/efi/sensor/sensor.h
        include/efi/sensor/sensor_type.h
        include/efi/sensor/sensor_reader.h
        include/efi/sensor/stored_value_sensor.h
        include/efi/sensor/converter_sensor.h
        include/efi/sensor/function_pointer_sensor.h
)

target_include_directories(lib
    PUBLIC
        include
)
