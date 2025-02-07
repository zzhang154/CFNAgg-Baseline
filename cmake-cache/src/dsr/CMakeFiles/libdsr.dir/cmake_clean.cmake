file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-dsr-default.pdb"
  "../../../build/lib/libns3.42-dsr-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libdsr.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
