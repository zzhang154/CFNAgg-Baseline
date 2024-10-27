file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-mobility-default.pdb"
  "../../../build/lib/libns3.42-mobility-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libmobility.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
