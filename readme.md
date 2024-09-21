1) Name: Darshith Karthikeyan

2) Student ID

3) Additional details

4) Code files:

    All static port numbers were strictly hardcoded as per the project guidelines.
    ServerEE.c- Runs over UDP port number 23105 
    ServerCS.c- Runs over UDP port number 22105
    ServerC.c- Runs over UDP port number 21105
        All the above three servers access the respective txt file and process the information through tokenization and other
        string operations. Based on the result of the query/operations the servers respond with a buffer/message.

    ServerM.c- Runs over TCP port 25105 and UDP port number 24105
        The main server has two parts. There is a function for all the UDP communications, where the server acts as the
        client and in the main function, the TCP server is implemented.

    client.c- Runs over TCP dynamic port numbers
        The client has a function for TCP communications and the main function is responsible for all user inputs and outputs.
    
    All test cases concerning the given requirements were successful to log the correct result. As per my testing, the 
    order in which the servers or clients are launched does not affect the functionality of the program.

5) Format of Messages:

    Authentication:
        1,Username,Password
            client -> main server
        Encrypted Username,Encrypted Password
            main server -> ServerC
        The server C sends the result of the authentication to the Main server
        and the main server relays it to the client.
    Querry:
        2,course,category,uername 
            Client -> Main Server
        Course,category
            Main Server -> EE/CS server
        All servers -> client(acting clients)
            Buffer message

6) idiosyncrasy

    No idiosyncrasies were detected during sanity testing.
    Will have to make sure about the tokenizing character ","(username, passwords, course, and category cannot have the character ",") 
    and " "(For multiple courses) while entering user inputs.

7) Reused Code

    https://beej.us/guide/bgnet/source/examples/listener.c
    https://beej.us/guide/bgnet/source/examples/client.c
    https://beej.us/guide/bgnet/source/examples/server.c
    https://www.geeksforgeeks.org/udp-server-client-implementation-c/
    All reused codes from reputable sources were completely understood and modified as per the project requirements.

Note: All files compile and run with no errors/warnings as expected with the provided VM.



