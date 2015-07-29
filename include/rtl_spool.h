#ifndef _RTL_SPOOL_H
#define _RTL_SPOOL_H


#define RTL_SPOOL_TEMP		0x01
#define RTL_SPOOL_WIND_SPEED	0x02
#define RTL_SPOOL_WIND_DIR	0x04
#define RTL_SPOOL_HUMIDITY	0x10
#define RTL_SPOOL_RAIN		0x20

struct rtl_spool_data {
  int fields;
  float temperature;
  int wind_speed;
  float wind_direction;
  int humidity;
  int rainfall_counter;
  // 1 or 2, for R1, R2
  int r_index;
};

int spool_callback(const struct rtl_spool_data *data);
int spool_init(const char *spool_dir);
int spool_shutdown(void);

#endif // _RTL_SPOOL_H
