rm ./server
rm ./client
clear
g++ tcpsocket.cpp link.cpp server.cpp -o server
g++ queue.c common.cpp client.cpp -pthread -o client
