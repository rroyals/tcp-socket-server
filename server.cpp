#include "server.hpp"

using namespace std;

#define SERVER_IP     "127.0.0.1"
#define SERVER_PORT   13374
#define MAX_CLIENTS   3

int main()
{

  int client_socket[MAX_CLIENTS], new_socket;

  int socket_dest, max_socket_dest;

  char buffer[4096];
  memset(buffer, 0, 4096);

  for (auto i = 0; i < MAX_CLIENTS; i++)
  {
    client_socket[i] = 0;
  }

  int socket_listen = socket(AF_INET, SOCK_STREAM, NULL);
  
  struct sockaddr_in address;

  address.sin_addr.s_addr = inet_addr(SERVER_IP);
  address.sin_family = AF_INET;
  address.sin_port = htons(SERVER_PORT);

  if (socket_listen == 0)
  {
    cout << "Socket creation failed." << endl;
    return 0;
  }

  if (bind(socket_listen, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    cout << "Binding socket failed." << endl;
    close(socket_listen);
    return 0;
  }

  if (listen(socket_listen, SOMAXCONN) < 0)
  {
    cout << "Listening socket failed." << endl;
    close(socket_listen);
    return 0;
  }

  socklen_t address_size = sizeof(address);

  cout << "SERVER :: Waiting for connections..." << endl;

  fd_set fd_master;
  

  while (true)
  {

    FD_ZERO(&fd_master);

    FD_SET(socket_listen, &fd_master);

    max_socket_dest = socket_listen;

    for (auto i = 0; i < MAX_CLIENTS; i++)
    {

      socket_dest = client_socket[i];

      if (socket_dest > 0)
        FD_SET (socket_dest, &fd_master);

      if (socket_dest > max_socket_dest)
        max_socket_dest = socket_dest;

    }

    int activity = select(max_socket_dest + 1, &fd_master, NULL, NULL, NULL);

    if ((activity < 0) && (errno!=EINTR))
    {
      printf("select error.");
    }

    if (FD_ISSET(socket_listen, &fd_master))
    {
      if ((new_socket = accept(socket_listen, (struct sockaddr*)&address, &address_size)) < 0)
      {
        cout << "Error accepting new connection." << endl;
      }

      printf("New Connection -> %s @ %i\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

      string s_welcome = "Welcome! Test!";

      send(new_socket, s_welcome.c_str(), s_welcome.length(), NULL);

      for (auto i = 0; i < MAX_CLIENTS; i++)
      {

        if (client_socket[i] == 0)
        {
          client_socket[i] = new_socket;
          //printf("Added Socket %i at index %i\n", new_socket, i);

          break;
          
        }
      }
    }

    for (auto i = 0; i < MAX_CLIENTS; i++)
    {
      
      socket_dest = client_socket[i];
      if (FD_ISSET(socket_dest, &fd_master))
      {
        
        int recv_data = recv(socket_dest, buffer, 4096, NULL);
        if (recv_data <= 0)
        {
          
          getpeername(socket_dest, (struct sockaddr*)&address, &address_size);
          printf("Disconect -> %s @ %i\n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

          close(socket_dest);
          client_socket[i] = 0;
          
        }
        else
        {
          send(socket_dest, buffer, strlen(buffer), NULL);
        }
      }
    }
  }
  return 0;
}
