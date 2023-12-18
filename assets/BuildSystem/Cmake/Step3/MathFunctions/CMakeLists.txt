add_library(MathFunctions MathFunctions.cxx)

# TODO 1: State that anybody linking to MathFunctions needs to include the
# current source directory, while MathFunctions itself doesn't.
# Hint: Use target_include_directories with the INTERFACE keyword
# 由于使用了INTERFACE关键字，主程序依赖MathFunctions库，会自动添加这个头文件地址
target_include_directories(MathFunctions
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                           )

# should we use our own math functions
option(USE_MYMATH "Use tutorial provided math implementation" ON)
if (USE_MYMATH)
  # 由于使用了PRIVATE关键字，表示依赖项只对当前目标有效，也就是"USE_MYMATH"只对生成该库的源文件有效
  target_compile_definitions(MathFunctions PRIVATE "USE_MYMATH")

  # library that just does sqrt
  add_library(SqrtLibrary STATIC
              mysqrt.cxx
              )

  target_link_libraries(MathFunctions PUBLIC SqrtLibrary)

  # TODO 6: Link MathFunctions to tutorial_compiler_flags
  target_link_libraries(SqrtLibrary PUBLIC tutorial_compiler_flags)

endif()

# TODO 7: Link SqrtLibrary to tutorial_compiler_flags
target_link_libraries(MathFunctions PUBLIC tutorial_compiler_flags)
