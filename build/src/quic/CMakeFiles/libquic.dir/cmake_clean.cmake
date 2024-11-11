file(REMOVE_RECURSE
  "../../lib/libns3.42-quic-debug.pdb"
  "../../lib/libns3.42-quic-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libquic.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
