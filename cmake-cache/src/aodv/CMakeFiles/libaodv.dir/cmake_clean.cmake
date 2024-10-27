file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-aodv-default.pdb"
  "../../../build/lib/libns3.42-aodv-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libaodv.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
