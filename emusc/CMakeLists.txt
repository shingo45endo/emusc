cmake_minimum_required(VERSION 3.8...3.24)

project(emusc-client VERSION 0.2.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

set(USE_MACOS_BUNDLE "no")

set(ALSA_AUDIO  "no")
set(CORE_AUDIO  "no")
set(JACK_AUDIO  "no")
set(PULSE_AUDIO "no")
set(QT_AUDIO    "no")
set(WAV_AUDIO   "no")
set(WIN32_AUDIO "no")

set(ALSA_MIDI  "no")
set(CORE_MIDI  "no")
set(WIN32_MIDI "no")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Gui OPTIONAL_COMPONENTS Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Gui OPTIONAL_COMPONENTS Multimedia)

if (Qt${QT_VERSION_MAJOR}Multimedia_FOUND)
  add_definitions(-D__QT_AUDIO__)
  set(QT_AUDIO "yes")
endif()

# TODO: Find libemusc with option to look for system or local

if (UNIX AND NOT APPLE)
  find_package(ALSA QUIET)
  if (ALSA_FOUND)
    list(APPEND EXTERNAL_INCLUDE_FILES ${ALSA_INCLUDE_DIRS})
    list(APPEND EXTERNAL_LIBRARIES ${ALSA_LIBRARIES})
    add_definitions(-D__ALSA_AUDIO__ -D__ALSA_MIDI__)
    set(ALSA_AUDIO "yes")
    set(ALSA_MIDI "yes")
  endif(ALSA_FOUND)

  find_package(PulseAudio QUIET)
  if (PULSEAUDIO_FOUND)
    list(APPEND EXTERNAL_INCLUDE_FILES ${PULSEAUDIO_INCLUDE_DIR})
    list(APPEND EXTERNAL_LIBRARIES ${PULSEAUDIO_LIBRARY})
    add_definitions(-D__PULSE_AUDIO__)
    set(PULSE_AUDIO "yes")
  endif()

  find_package(JACK QUIET)
  if (JACK_FOUND)
    list(APPEND EXTERNAL_INCLUDE_FILES ${JACK_INCLUDE_DIR})
    list(APPEND EXTERNAL_LIBRARIES ${JACK_LIBRARY})
    add_definitions(-D__JACK_AUDIO__)
    set(JACK_AUDIO "yes")
  endif()

# TODO: Check for libraries actually being available
elseif(WIN32)
  list(APPEND EXTERNAL_LIBRARIES winmm)
  add_definitions(-D__WIN32_AUDIO__ -D__WIN32_MIDI__)
  set(WIN32_AUDIO "yes")
  set(WIN32_MIDI "yes")

#  target_link_libraries(${target_name} winmm.lib)

elseif(APPLE)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  if (COREFOUNDATION_LIBRARY)
    list(APPEND EXTERNAL_LIBRARIES ${COREFOUNDATION_LIBRARY})
  else()
    message(ERROR "CoreFoundation framework not found, but is needed on macOS")
  endif()

  find_library(COREAUDIO_LIBRARY CoreAudio)
  find_library(AUDIOUNIT_LIBRARY AudioUnit)
  if (COREAUDIO_LIBRARY AND AUDIOUNIT_LIBRARY)
    list(APPEND EXTERNAL_LIBRARIES ${COREAUDIO_LIBRARY};${AUDIOUNIT_LIBRARY})
    add_definitions(-D__CORE_AUDIO__)
    set(CORE_AUDIO "yes")
  else()
    message(WARNING "CoreAudio framework not found, no audio output possible ")
  endif()

find_library(COREMIDI_LIBRARY CoreMIDI)
  if (COREMIDI_LIBRARY)
    list(APPEND EXTERNAL_LIBRARIES ${COREMIDI_LIBRARY})
    add_definitions(-D__CORE_MIDI__)
    set(CORE_MIDI "yes")
  else()
    message(WARNING "CoreMIDI framework not found, no external MIDI events")
  endif()

endif()

add_subdirectory(src)

if(APPLE)
  set_target_properties(emusc-client PROPERTIES MACOSX_BUNDLE TRUE)
  set(USE_MACOS_BUNDLE "yes")

elseif(WIN32)
  option(BUILD_WIN32_EXECUTABLE "Build executable without debug console" ON)
  set_target_properties(emusc-client PROPERTIES WIN32_EXECUTABLE ${BUILD_WIN32_EXECUTABLE})
endif()

# Currently all targets are expected to support WAV output
set(WAV_AUDIO "yes")
target_compile_definitions(emusc-client PUBLIC __WAV_AUDIO__)

install(TARGETS emusc-client DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES AUTHORS ChangeLog COPYING NEWS README.md
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/emusc)

message(STATUS "")
message(STATUS " EmuSC client interface summary: ")
message(STATUS "-===============================-")
message(STATUS "")

if(APPLE)
  message(STATUS "Platform: macOS")
  message(STATUS " * Build bundle: ${USE_MACOS_BUNDLE}")
elseif(UNIX)
  message(STATUS "Platform: UNIX")
elseif(WIN32)
  message(STATUS "Platform: Windows")
  message(STATUS " * Debug console: ${WIN32_EXECUTABLE}")
else()
  message(STATUS "Platform: unkown")  
endif()

message(STATUS "")
message(STATUS "Qt version: Qt${QT_VERSION_MAJOR}")

message(STATUS "")
message(STATUS "Audio APIs:")
message(STATUS " * ALSA  : ${ALSA_AUDIO}")
message(STATUS " * Core  : ${CORE_AUDIO}")
message(STATUS " * Jack  : ${JACK_AUDIO}")
message(STATUS " * Null  : yes")
message(STATUS " * Pulse : ${PULSE_AUDIO}")
message(STATUS " * Qt    : ${QT_AUDIO}")
message(STATUS " * WAV   : ${WAV_AUDIO}")
message(STATUS " * Win32 : ${WIN32_AUDIO}")
message(STATUS "")
message(STATUS "MIDI APIs:")
message(STATUS " * ALSA  : ${ALSA_MIDI}")
message(STATUS " * Core  : ${CORE_MIDI}")
message(STATUS " * Win32 : ${WIN32_MIDI}")
message(STATUS "")
message(STATUS "Make sure all needed APIs are found before you continue")
message(STATUS "")
