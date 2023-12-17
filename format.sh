find src/ -iname *.h -o -iname *.hpp -o -iname *.c -o -iname *.cpp | xargs clang-format.exe -i
find include/ -iname *.h -o -iname *.hpp -o -iname *.c -o -iname *.cpp | xargs clang-format.exe -i
find benchmark/ -iname *.h -o -iname *.hpp -o -iname *.c -o -iname *.cpp | xargs clang-format.exe -i
find mock/ -iname *.h -o -iname *.hpp -o -iname *.c -o -iname *.cpp | xargs clang-format.exe -i
find unit_test/ -iname *.h -o -iname *.hpp -o -iname *.c -o -iname *.cpp | xargs clang-format.exe -i

