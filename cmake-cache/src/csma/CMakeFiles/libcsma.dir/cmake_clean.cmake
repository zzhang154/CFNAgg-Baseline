file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-csma-default.pdb"
  "../../../build/lib/libns3.42-csma-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libcsma.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
