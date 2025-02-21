file(REMOVE_RECURSE
  "../../lib/libns3.42-internet-apps-debug.pdb"
  "../../lib/libns3.42-internet-apps-debug.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/libinternet-apps.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
