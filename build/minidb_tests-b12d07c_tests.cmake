add_test( [==[Catch2 is working]==] /Users/yulepark/Projects/miniDB/build/minidb_tests [==[Catch2 is working]==]  )
set_tests_properties( [==[Catch2 is working]==] PROPERTIES WORKING_DIRECTORY /Users/yulepark/Projects/miniDB/build SKIP_RETURN_CODE 4)
set( minidb_tests_TESTS [==[Catch2 is working]==])
