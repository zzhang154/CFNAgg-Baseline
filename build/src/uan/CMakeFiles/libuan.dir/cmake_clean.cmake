file(REMOVE_RECURSE
  "../../lib/libns3.42-uan-debug.pdb"
  "../../lib/libns3.42-uan-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libuan.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
