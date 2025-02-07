file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-config-store-default.pdb"
  "../../../build/lib/libns3.42-config-store-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libconfig-store.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
