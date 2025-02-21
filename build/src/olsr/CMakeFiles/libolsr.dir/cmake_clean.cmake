file(REMOVE_RECURSE
  "../../lib/libns3.42-olsr-debug.pdb"
  "../../lib/libns3.42-olsr-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libolsr.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
