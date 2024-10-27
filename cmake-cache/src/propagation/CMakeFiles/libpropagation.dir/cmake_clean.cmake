file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-propagation-default.pdb"
  "../../../build/lib/libns3.42-propagation-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libpropagation.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
