test: server_methods server_use
	echo "done"
server_use:
	./server.rb
server_methods:
	D=/tmp/store_server_methods ;mkdir $D;node etc.js; rm -rf $D
