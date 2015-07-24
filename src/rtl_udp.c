#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "rtl_udp.h"

static int rtl_socket = -1;
static struct sockaddr_in rtl_broadcastAddr;

int udp_callback(const struct rtl_udp_data *data)
{
	return -1;
}

int udp_init_socket(void)
{
	if(rtl_socket != -1) {
		return 0;
	}

    rtl_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( rtl_socket <= 0 ) {
    	perror("could not open socket.");
        return 0;
    }
    
    int broadcastEnable = 1;
    int ret = setsockopt(rtl_socket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret) {
    	perror("unable to set broadcase mode.");
        close(rtl_socket);
        rtl_socket = -1;
        return 0;
    }
    
    memset(&rtl_broadcastAddr, 0, sizeof rtl_broadcastAddr);
    rtl_broadcastAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "239.255.255.255", &rtl_broadcastAddr.sin_addr); // Set the broadcast IP address
    rtl_broadcastAddr.sin_port = htons(4532); // Set port 1900	
	
	return 1;
}

int udp_destroy_socket(void)
{
	if(rtl_socket != -1) {
		close(rtl_socket);
		rtl_socket = -1;
		return 1;
	}

	return -1;
}