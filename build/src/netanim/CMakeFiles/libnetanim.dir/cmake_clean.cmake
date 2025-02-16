file(REMOVE_RECURSE
  "../../lib/libns3.42-netanim-debug.pdb"
  "../../lib/libns3.42-netanim-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libnetanim.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
