file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-dsdv-default.pdb"
  "../../../build/lib/libns3.42-dsdv-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libdsdv.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
