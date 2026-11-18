list(APPEND SOURCE
    feedback.cpp)

add_executable(feedback ${SOURCE})
set_module_type(feedback win32gui UNICODE)
add_importlibs(feedback user32 gdi32 comctl32 comdlg32 shell32 msvcrt kernel32)
add_cd_file(TARGET feedback DESTINATION reactos FOR all)
add_cd_file(TARGET feedback DESTINATION reactos/system32 FOR all)
