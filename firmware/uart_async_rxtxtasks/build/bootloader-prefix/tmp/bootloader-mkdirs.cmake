# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Rijin/esp/esp-idf/components/bootloader/subproject"
  "C:/Users/Rijin/Documents/Projects/MURAL-i/firmware/uart_async_rxtxtasks/build/bootloader"
  "C:/Users/Rijin/Documents/Projects/MURAL-i/firmware/uart_async_rxtxtasks/build/bootloader-prefix"
  "C:/Users/Rijin/Documents/Projects/MURAL-i/firmware/uart_async_rxtxtasks/build/bootloader-prefix/tmp"
  "C:/Users/Rijin/Documents/Projects/MURAL-i/firmware/uart_async_rxtxtasks/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/Rijin/Documents/Projects/MURAL-i/firmware/uart_async_rxtxtasks/build/bootloader-prefix/src"
  "C:/Users/Rijin/Documents/Projects/MURAL-i/firmware/uart_async_rxtxtasks/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Rijin/Documents/Projects/MURAL-i/firmware/uart_async_rxtxtasks/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Rijin/Documents/Projects/MURAL-i/firmware/uart_async_rxtxtasks/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
