#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "rtl_433.h"
#include "rtl_spool.h"

#define VERSION_STR "1.0"
#define DEFAULT_DIR "/var/spool/rtl_433"

static char *g_spool = NULL;

int spool_callback(const struct rtl_spool_data *data)
{
  char buf[1024];
  char field[128];
  int  cnt = 1024;
  int len;
  char *path_src;
  char *path_dst;
  FILE *fl;

  // no data
  if(data->fields == 0x00) {
    return 0;
  }

  if(g_spool == NULL || strlen(g_spool) == 0) {
    return 0;
  }

  strncpy(buf, "{", cnt);
  cnt -= (strlen("{") + 1);
  
  if(data->fields & RTL_SPOOL_TEMP) {
    snprintf(field, 128, "\"temperature\": %3.2f,", data->temperature);
    strncat(buf, field, cnt);
    cnt -= (strlen(buf) + 1);
	}
  if(data->fields & RTL_SPOOL_WIND_SPEED) {
    snprintf(field, 128, "\"wind_speed\": %d,", data->wind_speed);
    strncat(buf, field, cnt);
    cnt -= (strlen(buf) + 1);
  }
  if(data->fields & RTL_SPOOL_WIND_DIR) {
    snprintf(field, 128, "\"wind_dir\": %3.2f,", data->wind_direction);
    strncat(buf, field, cnt);
    cnt -= (strlen(buf) + 1);
  }
  if(data->fields & RTL_SPOOL_HUMIDITY) {
    snprintf(field, 128, "\"humidity\": %d,", data->humidity);
    strncat(buf, field, cnt);
    cnt -= (strlen(buf) + 1);
  }
  if(data->fields & RTL_SPOOL_RAIN) {
    snprintf(field, 128, "\"rain\": %d,", data->rainfall_counter);
    strncat(buf, field, cnt);
    cnt -= (strlen(buf) + 1);
  }
  snprintf(field, 128, "\"spool_rtl\": %s", VERSION_STR);
  strncat(buf, field, cnt);
  cnt -= (strlen(buf) + 1);
  
  strncat(buf, "}", cnt);
  cnt -= (strlen("}") + 1);
  
  if(debug_output) {
    fprintf(stdout, "%s\n", buf);
  }

  // + "/R1.tmp"
  len = strlen(g_spool) + 16;
  path_src = (char *)malloc(len);
  if(path_src == NULL)
    return 0;

  path_dst = (char *)malloc(len);
  if(path_dst == NULL) {
    free(path_src);
    return 0;
  }

  snprintf(path_src, len, "%s/R%d.tmp", g_spool, data->r_index);
  snprintf(path_dst, len, "%s/R%d", g_spool, data->r_index);
  
  fl = fopen(path_src, "w");
  
  if(fl != NULL) {
    fwrite(buf, strlen(buf), 1, fl);
    fclose(fl);
    rename(path_src, path_dst);
  }
  
  free(path_src);
  free(path_dst);
  return 1;
}

int spool_init(const char *spool_dir)
{
  if(g_spool != NULL) {
    free(g_spool);
  }
  if(spool_dir != NULL) {
    g_spool = strdup(spool_dir);
  }
  else {
    g_spool = strdup(DEFAULT_DIR);
  }
  return 1;
}

int spool_shutdown(void)
{
  if(g_spool != NULL) {
    free(g_spool);
    g_spool = NULL;
  }
  return 1;
}
