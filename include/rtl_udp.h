#ifndef _RTL_UDP_H
#define _RTL_UDP_H

// bit mask for each field
// get types for each field
//	temp float
//	wind speed int
//  wind direction float
//  humidity int
//  rainfall counter int
// callback
// register
// deregister
// thread?

#define RTL_UDP_TEMP		0x01
#define RTL_UDP_WIND_SPEED	0x02
#define RTL_UDP_WIND_DIR	0x04
#define RTL_UDP_HUMIDITY	0x10
#define RTL_UDP_RAIN		0x20

struct rtl_udp_data {
	int fields;
	float temperature;
	int window_speed;
	float wind_direction;
	int humidity;
	int rainfall_counter;
};

int udp_callback(const struct *rtl_udp_data);

int udp_init_socket(void);
int udp_destroy_socket(void);

#endif // _RTL_UDP_H