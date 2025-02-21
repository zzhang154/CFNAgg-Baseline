file(REMOVE_RECURSE
  "../../lib/libns3.42-wimax-debug.pdb"
  "../../lib/libns3.42-wimax-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libwimax.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
