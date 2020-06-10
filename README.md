# RTOS
assignment 1
run the following commands                         
gcc server.c -o server -lpthread                       
./server 1 8000                          
open another terminal                                       
gcc client.c -o client -lpthread              
./client 2 127.0.0.1 8000                  

for group chat
gcc group_server.c -o server -lpthread
./server 8001
open another terminal 
gcc group_client.c -o client -lpthread
./client 4 127.0.0.1 8002 Harsha group

for Mid term project
one_v_one
gcc server_voice.c -o server -lpulse-simple -lpulse -lpthread
./server 8002
gcc client_voice.c -o client -lpulse-simple -lpulse -lpthread
./client 127.0.0.1 8888 Harsha


for group audio
gcc server_grp.c -o server -lpulse-simple -lpulse -lpthread
./server 8002
gcc client_grp.c -o client -lpulse-simple -lpulse -lpthread
./client 127.0.0.1 8002 Harsha group
