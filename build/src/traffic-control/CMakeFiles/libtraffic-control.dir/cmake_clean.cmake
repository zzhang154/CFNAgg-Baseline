file(REMOVE_RECURSE
  "../../lib/libns3.42-traffic-control-debug.pdb"
  "../../lib/libns3.42-traffic-control-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libtraffic-control.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
