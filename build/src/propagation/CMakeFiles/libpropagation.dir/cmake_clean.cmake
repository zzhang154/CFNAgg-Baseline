file(REMOVE_RECURSE
  "../../lib/libns3.42-propagation-debug.pdb"
  "../../lib/libns3.42-propagation-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libpropagation.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
