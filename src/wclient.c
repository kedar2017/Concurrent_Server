//
// client.c: A very, very primitive HTTP client.
// 
// To run, try: 
//      client hostname portnumber filename
//
// Sends one HTTP request to the specified HTTP server.
// Prints out the HTTP response.
//
// For testing your server, you will want to modify this client.  
// For example:
// You may want to make this multi-threaded so that you can 
// send many requests simultaneously to the server.
//
// You may also want to be able to request different URIs; 
// you may want to get more URIs from the command line 
// or read the list from a file. 
//
// When we test your server, we will be using modifications to this client.
//

#include "io_helper.h"
#include "thread.h"
#include <stdio.h>

#define MAXBUF (8192)

static pthread_mutex_t printf_mutex;
static pthread_mutex_t sendf_mutex;

typedef struct client_bundle cbundle_t;

struct client_bundle{

  int port;
  char* host;
  char* filename;
};

//
// Send an HTTP request for the specified file 
//
void client_send(int fd, char *filename) {
    char buf[MAXBUF];
    char hostname[MAXBUF];
    
    gethostname_or_die(hostname, MAXBUF);
    
    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    write_or_die(fd, buf, strlen(buf));
}

//
// Read the HTTP response and print it out
//
void client_print(int fd) {
    char buf[MAXBUF];  
    int n;
    
    pthread_mutex_lock(&printf_mutex);
    // Read and display the HTTP Header 
    n = readline_or_die(fd, buf, MAXBUF);
    
    while (strcmp(buf, "\r\n") && (n > 0)) {
    
	printf("Header: %s", buf);
	n = readline_or_die(fd, buf, MAXBUF);
	
	// If you want to look for certain HTTP tags... 
	// int length = 0;
	//if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
	//    printf("Length = %d\n", length);
	//}
    }
    
    // Read and display the HTTP Body 
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0) {
	printf("%s", buf);
	n = readline_or_die(fd, buf, MAXBUF);
    }
    pthread_mutex_unlock(&printf_mutex);
}

void* client_handle_t(void* arg) {
    
    int clientfd;
    pthread_mutex_lock(&sendf_mutex);
    cbundle_t* bundle = ((cbundle_t*)arg);
    
    int port = bundle->port;
    char* host = bundle->host;
    char* filename = bundle->filename;
    
    /* Open a single connection to the specified host and port */
    clientfd = open_client_fd_or_die(host, port);
    
    /* From here onwards should appear inside the thread function*/
    client_send(clientfd, filename);
    client_print(clientfd);
    
    close_or_die(clientfd);
    pthread_mutex_unlock(&sendf_mutex);
    
    return (NULL);
}


int main(int argc, char *argv[]) {
    
    pool_t* pool = pool_init(8000, 15);
    
    pthread_mutex_init(&printf_mutex, NULL);
    pthread_mutex_init(&sendf_mutex, NULL);
    
    for (int i =0; i<14; i++) {
        
        if (argc != 4) {
        fprintf(stderr, "Usage: %s <host> <port> <filename>\n", argv[0]);
        exit(1);
        }
        
        cbundle_t* bundle = malloc(sizeof(cbundle_t));
        bundle->port = atoi(argv[2]);
        bundle->host = argv[1];
        bundle->filename = argv[3];
        
        pool_add_job(pool, client_handle_t, bundle);
        
        //free(bundle);
    }
    
    while (1) {
        
    }
    
    exit(0);
}
