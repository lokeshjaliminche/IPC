#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int
main(int argc, char *argv[])
{
	struct sockaddr_un name;

#if 0
	struct sockaddr_un {
		sa_family_t sun_family;
		char	    sun_path[108];
	};
#endif

	int ret;
	int connection_socket;
	int data_socket;
	int result;
	int data;
	char buffer[BUFFER_SIZE];


	unlink(SOCKET_NAME);

	/* create a master socket. */

	connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);

	if (connection_socket == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	printf("master socket created %d\n", connection_socket);

	memset(&name, 0, sizeof(struct sockaddr_un));

	name.sun_family = AF_UNIX;
	strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

	if (setsockopt(connection_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&name, sizeof(struct sockaddr_un)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	ret = bind(connection_socket, (const struct sockaddr *) &name, sizeof(struct sockaddr_un));

	if (ret == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	printf("bind call succeded\n");

	ret = listen(connection_socket, 20);

	if (ret == -1)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}


	for(;;)
	{
		printf("waiting on accept sys call\n");

		data_socket = accept(connection_socket, NULL, NULL);
		if (data_socket == -1)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		
		printf("connection accepted from client\n");

		result = 0;
		for(;;)
		{
			memset(buffer, 0, BUFFER_SIZE);

			printf("waiting from data from the client\n");
			ret = read(data_socket, buffer, BUFFER_SIZE);
			if (ret == -1)
			{
				perror("read");
				exit(EXIT_FAILURE);
			}

			if (data == 0) break;
			result = result + data;
		}
		memcpy(&data, buffer, BUFFER_SIZE);
		sprintf(buffer, "Result = %d\n", result);
		printf("Sending final result to client\n");
		ret = read(data_socket, buffer, BUFFER_SIZE);
		if (ret == -1)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
		close(data_socket);
	}

	close(connection_socket);
	printf("connection closed\n");

	unlink(SOCKET_NAME);
	exit(EXIT_SUCCESS);

}
