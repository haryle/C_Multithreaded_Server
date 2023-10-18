deps = utils/node.c utils/linked_list.c utils/map.c utils/string_vector.c utils/list.c src/server.c


compile_server:
	gcc -Wall $(deps) assignment3.c -o out/server.o -pthread
