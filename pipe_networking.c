#include "pipe_networking.h"

#define CTS "cts"
#define STC "stc"
#define CTSM "client to server"
#define STCM "server to client"
#define CM "confirm previous step"

/*=========================
  server_handshake
  args: int * to_client

  Perofrms the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/

int server_handshake(int *to_client) {

  mkfifo(STC,0644); //server to client
  mkfifo(CTS,0644); //client to server
  int cts, stc;
  char tsm[256];

  if((cts = open(CTS, O_RDONLY | O_CREAT, 0600)) == -1) {
    printf("[server] can't open well known pipe\n");
  }
  printf("[server] opened well known pipe\n");

  read(cts, tsm, sizeof(tsm));
  printf("[server] client to server message read\n");

  if(strcmp(tsm,CTSM)) {
    printf("[server] client to server message check failed\n");
  }
  printf("[server] client to server message checked\n");
  
  if((stc = open(STC, O_WRONLY | O_CREAT, 0600)) == -1) {
    printf("[server] can't open to client pipe\n");
  }
  printf("[server] opened to client pipe\n");
  *to_client = stc;

  write(stc,STCM,sizeof(STCM));  
  printf("[server] server to client message sent\n");

  read(cts, tsm, sizeof(tsm));
  printf("[server] client to server confirmation read\n");

  if(strcmp(tsm,CM)) {
    printf("[server] client to server confirmation check failed\n");
  }
  printf("[server] client to server confirmation checked\n");

  remove("stc");
  printf("[server] server to client named pipe removed\n");  

  printf("[server] success\n");

  return cts;
}


/*=========================
  client_handshake
  args: int * to_server

  Perofrms the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {

  int cts, stc;
  char tcm[256];

  if((cts = open(CTS, O_WRONLY)) == -1) {
    printf("[client] can't open well known pipe\n");
  }
  printf("[client] opened well known pipe\n");

  *to_server = cts;

  write(cts, CTSM, sizeof(CTSM));
  printf("[client] client to server message sent\n");

  if((stc = open(STC, O_RDONLY)) == -1) {
    printf("[client] can't open to client pipe\n");
  }
  printf("[client] opened to client pipe\n");
  
  read(stc,tcm,sizeof(tcm));
  printf("[client] server to client confirmation read\n");

  if(strcmp(tcm,STCM)) {
    printf("[client] server to client confirmation check failed\n");
  }
  printf("[client] server to client confirmation checked\n");

  remove("cts");
  printf("[client] client to server named pipe removed\n");
  
  write(cts,CM,sizeof(CM));
  printf("[client] client to server confirmation sent\n");

  printf("[client] success\n");

  return cts;
}

