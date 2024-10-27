file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-applications-default.pdb"
  "../../../build/lib/libns3.42-applications-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libapplications.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
