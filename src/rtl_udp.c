#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "rtl_433.h"
#include "rtl_udp.h"

static int rtl_socket = -1;
static struct sockaddr_in rtl_broadcastAddr;


#define VERSION_STR "1.0"
int udp_callback(const struct rtl_udp_data *data)
{
	char buf[1024];
	char field[128];
	int  cnt = 1024;

	// no data
	if(data->fields == 0x00) {
		return 0;
	}
	strncpy(buf, "{", cnt);
	cnt -= (strlen("{") + 1);

	if(data->fields & RTL_UDP_TEMP) {
		snprintf(field, 128, "\"temperature\": %3.2f,", data->temperature);
		strncat(buf, field, cnt);
		cnt -= (strlen(buf) + 1);
	}
	if(data->fields & RTL_UDP_WIND_SPEED) {
		snprintf(field, 128, "\"wind_speed\": %d,", data->wind_speed);
		strncat(buf, field, cnt);
		cnt -= (strlen(buf) + 1);
	}
	if(data->fields & RTL_UDP_WIND_DIR) {
		snprintf(field, 128, "\"wind_dir\": %3.2f,", data->wind_direction);
		strncat(buf, field, cnt);
		cnt -= (strlen(buf) + 1);
	}
	if(data->fields & RTL_UDP_HUMIDITY) {
		snprintf(field, 128, "\"humidity\": %d,", data->humidity);
		strncat(buf, field, cnt);
		cnt -= (strlen(buf) + 1);
	}
	if(data->fields & RTL_UDP_RAIN) {
		snprintf(field, 128, "\"rain\": %d,", data->rainfall_counter);
		strncat(buf, field, cnt);
		cnt -= (strlen(buf) + 1);
	}
	snprintf(field, 128, "\"udp_rtl\": %s", VERSION_STR);
	strncat(buf, field, cnt);
	cnt -= (strlen(buf) + 1);
	
	strncat(buf, "}", cnt);
	cnt -= (strlen("}") + 1);
	
	if(debug_output) {
		fprintf(stdout, "%s\n", buf);
	}
	int ret = sendto(rtl_socket, buf, strlen(buf), 0, (struct sockaddr*)&rtl_broadcastAddr, sizeof rtl_broadcastAddr);
    if (ret < 0) {
    	perror("sendto failed");
        close(rtl_socket);
        rtl_socket = -1;
        return 0;        
    }

    if(debug_output) {
	    printf("sent %d bytes to %s\n", ret,
    	    inet_ntoa(rtl_broadcastAddr.sin_addr));
  	}
	return 1;
}

int udp_init_socket(const char *addr, int port)
{
	if(rtl_socket != -1) {
		perror("socket already exists");
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
    	perror("unable to set broadcast mode.");
        close(rtl_socket);
        rtl_socket = -1;
        return 0;
    }
    struct hostent *he = gethostbyname(addr);
    memset(&rtl_broadcastAddr, 0, sizeof rtl_broadcastAddr);
    rtl_broadcastAddr.sin_family = AF_INET;
	rtl_broadcastAddr.sin_addr = *((struct in_addr *)he->h_addr);
    
    memset(rtl_broadcastAddr.sin_zero, '\0', sizeof rtl_broadcastAddr.sin_zero);
    
    rtl_broadcastAddr.sin_port = htons(port); 

	fprintf(stdout, "udp broadcasting on %d\n", port);
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