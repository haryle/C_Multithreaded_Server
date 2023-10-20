deps = node.c linked_list.c map.c string_vector.c list.c server.c


compile_server:
	gcc -Wall assignment3.c $(deps)  -o assignment3 -pthread -std=c99

