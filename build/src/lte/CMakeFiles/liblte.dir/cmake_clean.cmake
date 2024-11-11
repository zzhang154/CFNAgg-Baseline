file(REMOVE_RECURSE
  "../../lib/libns3.42-lte-debug.pdb"
  "../../lib/libns3.42-lte-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/liblte.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
