briqs: main.cpp
briqs: 0_core.cpp
briqs: 1_gc.cpp
briqs: 2_db.cpp
briqs: 3_facade.cpp
briqs: 4_query.cpp
briqs: 5-1_lexer.cpp
briqs: 5-2_parser.cpp
briqs: 5-3_stiq.cpp
briqs: 5-4_core_operators.cpp
briqs: 5-5_functions.cpp
briqs: 6_uuid.cpp
	clang++ -std=c++11 $^ -o $@ -g -O0
