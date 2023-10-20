deps = node.c linked_list.c map.c string_vector.c list.c server.c


compile_server:
	gcc -Wall assignment3.c $(deps)  -o out/assignment3.o -pthread -std=c99

