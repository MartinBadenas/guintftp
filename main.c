#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>

#define DEFAULT_PORT	69

int main(int argc, char *argv[]){
	int sd;
	int port=DEFAULT_PORT;
	struct sockaddr_in addr;
	char buffer[516];

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
		perror("bind");
	while (1) {
		int bytes, addr_len=sizeof(addr);

		bytes = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);
		printf("msg from %s:%d (%d bytes)\n", inet_ntoa(addr.sin_addr),
						ntohs(addr.sin_port), bytes);
		
	}
	close(sd);
}

