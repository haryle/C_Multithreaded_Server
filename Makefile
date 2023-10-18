deps = utils/node.c utils/linked_list.c utils/map.c utils/string_vector.c utils/list.c src/server.c


compile_server:
	gcc -Wall assignment3.c $(deps)  -o out/server.o -pthread -std=c99
