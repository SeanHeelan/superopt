#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>

using namespace std;

#define FORMULA_SHM_KEY 224
#define RESULTS_SHM_KEY 46
#define FORMULA_SIZE_BYTES 65536
#define RESULT_SIZE_BYTES 4096

#define PORT 8001

string write_problem_to_z3server(string formula) {
  int sock = 0, nread, nchars, total_read;
  struct sockaddr_in serv_addr;
  char form_buffer[FORMULA_SIZE_BYTES+1] = {0};
  char res_buffer[RESULT_SIZE_BYTES+1] = {0};

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("z3client: socket creation failed");
    return NULL;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    perror("z3client: Invalid localhost network address");
    return NULL;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))
      < 0) {
    perror("z3client: connect() to z3server failed");
    return NULL;
  }

  /* Send the formula to the server */
  cout << "Writing formula " <<  formula << endl;
  nchars = std::min(FORMULA_SIZE_BYTES, (int)formula.length());
  strncpy(form_buffer, formula.c_str(), nchars);
  send(sock, form_buffer, nchars, 0);
  /* Read back solver results. */
  total_read = 0;
  do {
    nread = read(sock, res_buffer, RESULT_SIZE_BYTES - total_read);
    total_read += nread;
  } while (res_buffer[total_read] != '\0');
  return string(res_buffer);
}
