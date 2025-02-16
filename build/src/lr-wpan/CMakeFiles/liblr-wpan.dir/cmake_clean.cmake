file(REMOVE_RECURSE
  "../../lib/libns3.42-lr-wpan-debug.pdb"
  "../../lib/libns3.42-lr-wpan-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/liblr-wpan.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
