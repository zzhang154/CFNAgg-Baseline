file(REMOVE_RECURSE
  "../../lib/libns3.42-core-debug.pdb"
  "../../lib/libns3.42-core-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libcore.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
