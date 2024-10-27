file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-traffic-control-default.pdb"
  "../../../build/lib/libns3.42-traffic-control-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libtraffic-control.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
