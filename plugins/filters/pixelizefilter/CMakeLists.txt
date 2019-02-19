set(RUST_CARGO_EXECUTABLE "D:\\dev\\toolchain\\rust\\cargo\\bin\\cargo.exe")

# FIXME: Specify target only for Windows (mingw)
# FIXME: Detect bitness
set(RUST_TARGET "x86_64-pc-windows-gnu")
# FIXME: ...
list(APPEND RUST_NATIVE_STATIC_LIBS userenv)

set(RUST_TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/rust-target")
# Note: `rustc` does *not* output libxxx.a on Windows even with the GNU target.
# FIXME: look for proper file on *nix
set(krita_filter_pixelize_rs_OUTLIB "${RUST_TARGET_DIR}/x86_64-pc-windows-gnu/debug/krita_filter_pixelize_rs.lib")

# Dummy target to cause `cargo` to always run
add_custom_command(OUTPUT always_build COMMAND ${CMAKE_COMMAND} -E echo_append)

# FIXME: Add `--release` for release build
add_custom_command(
    OUTPUT "${krita_filter_pixelize_rs_OUTLIB}"
    COMMAND "${RUST_CARGO_EXECUTABLE}" ARGS build --target x86_64-pc-windows-gnu --target-dir "${RUST_TARGET_DIR}"
    DEPENDS always_build
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rust"
    USES_TERMINAL
)

add_custom_target(krita_filter_pixelize_rs DEPENDS "${krita_filter_pixelize_rs_OUTLIB}")


set(kritapixelizefilter_SOURCES kis_pixelize_filter_plugin.cpp kis_pixelize_filter.cpp )
add_library(kritapixelizefilter MODULE ${kritapixelizefilter_SOURCES})
target_compile_definitions(kritapixelizefilter PRIVATE -DUSE_RUST)
add_dependencies(kritapixelizefilter krita_filter_pixelize_rs)
target_link_libraries(kritapixelizefilter PRIVATE kritaui "${krita_filter_pixelize_rs_OUTLIB}" ${RUST_NATIVE_STATIC_LIBS})
install(TARGETS kritapixelizefilter  DESTINATION ${KRITA_PLUGIN_INSTALL_DIR})
