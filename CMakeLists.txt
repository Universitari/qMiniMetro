project(qMiniMetro)

# CMake 3.* necessario per Qt
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# ottengo tutti i sorgenti
file(GLOB my_sources *.h *.cpp *.qrc *.ui)

# attivo funzionalita' specifiche di CMake per Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# aggiungo eseguibile
add_executable(qMiniMetro WIN32 main.cpp ${my_sources})

# verifico l'esistenza delle librerie Qt necessarie
find_package(Qt5Widgets REQUIRED)

# collego le librerie Qt con l'eseguibile
target_link_libraries(qMiniMetro Qt5::Widgets)

# installo l'eseguibile in "bin"

install (TARGETS qMiniMetro DESTINATION bin)

# creo un pacchetto di installazione v0.1.0

set (CPACK GENERATOR "TGZ")

set (CPACK PACKAGE VERSION MAJOR 0 )
set (CPACK PACKAGE VERSION MINOR 1 )
set (CPACK PACKAGE VERSION PATCH 2 )

include (CPack )

