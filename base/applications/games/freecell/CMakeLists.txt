list(APPEND SOURCE
    freecell.cpp
    freecell.h
    freecell_game.cpp
    freecell_create.cpp)

add_rc_deps(rsrc.rc ${CMAKE_CURRENT_SOURCE_DIR}/freecell.ico)

add_executable(freecell
    ${SOURCE}
    rsrc.rc)

target_link_libraries(freecell cardlib)

set_module_type(freecell win32gui UNICODE)

add_importlibs(freecell
    advapi32
    comctl32
    shell32
    user32
    gdi32
    msvcrt
    kernel32)

add_pch(freecell freecell.h SOURCE)

add_cd_file(
    TARGET freecell
    DESTINATION reactos/system32
    FOR all)
