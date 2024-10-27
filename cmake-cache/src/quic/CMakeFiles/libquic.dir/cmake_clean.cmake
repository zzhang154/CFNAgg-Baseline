file(REMOVE_RECURSE
  "../../../build/lib/libns3.42-quic-default.pdb"
  "../../../build/lib/libns3.42-quic-default.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libquic.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
