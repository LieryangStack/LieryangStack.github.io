TARGET = helloworld

HEADERS = hello.h

SOURCES = hello.cpp \
          main.cpp

win32 {
  message(Building with OpenGL support.)
}
