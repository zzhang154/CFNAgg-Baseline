file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-olsr-default.pdb"
  "../../../build/lib/libns3.42-olsr-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libolsr.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
