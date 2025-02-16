file(REMOVE_RECURSE
  "../../lib/libns3.42-dsr-debug.pdb"
  "../../lib/libns3.42-dsr-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libdsr.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
