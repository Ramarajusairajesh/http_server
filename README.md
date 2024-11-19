I started learning networking quite few years ago  out of curiosity ! I believe I understood few things along my 2 years of self study! So rather than saying that I know the topic by theory it felt "writting my own code in the languages I know " better!

So I started this repo not only for a simple htt_server , but gonna update it with smb server / ftp server with authentication etc ! 

This repo work for linux but for windows you need to change the socket library with winsock api to access sockets in windows!


Simple explanation of what libraries I used in these programs: 

For C:
      For the C implementation I used socket,unistd libraries to define the socket interface to crete socket , transmit data via socket and recieve data via socket
      
For python:
        Similar to C used socket interface and add used mimetypes to define file types rather than writing my own dictionary! Downloaded a simple and free template that and hosten it via http server!



    For python3 you need to install the following libraries:
            socket 
            datetime
            mimetypes

          Command to install these libraries: pip install socket datetime mimetypes

Future plans :  Make this http v/2.0 and make encryption!

Currently working : Completing the C http server and adding rust http server along with it !
