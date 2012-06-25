/*
 * Copyright (c) 2011 Thomas de Grivel <thomas@lowh.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <errno.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/sensors.h>

int find_sensordev (const char *name)
{
  int mib[3] = { CTL_HW, HW_SENSORS, 0 };
  size_t len;
  struct sensordev d;

  while (mib[2] < 100) {
    //printf("find_sensordev(%s) %d\n", name, mib[2]);
    if (sysctl(mib, 3, NULL, &len, NULL, 0) == -1)
      err(1, "sysctl 11");
    if (len != sizeof(d))
      err(1, "incompatible size for struct sensordev: %lu instead of %lu",
	  len, sizeof(d));
    if (sysctl(mib, 3, &d, &len, NULL, 0) == -1)
      err(1, "sysctl 12");
    if (strcmp(name, d.xname) == 0)
      return mib[2];
    mib[2]++;
  }
  return 0;
}

int read_sensor (int64_t *value, int dev, int type, int sensor, const char *name)
{
  int mib[5] = { CTL_HW, HW_SENSORS, dev, type, sensor };
  size_t len;
  struct sensor s;

  if (sysctl(mib, 5, NULL, &len, NULL, 0) == -1) {
    //warn(name);
    return 1;
  }
  if (len != sizeof(s))
    err(1, "incompatible size for struct sensor: %lu instead of %lu",
	len, sizeof(s));
  if (sysctl(mib, 5, &s, &len, NULL, 0) == -1)
    err(1, name);
  //printf("%9llu %s\n", s.value, s.desc);
  *value = s.value;
  return 0;
}

typedef struct batinfo {
  int64_t ac;
  int64_t remaining;
  int64_t current;
  int64_t full;
  int64_t low;
} s_batinfo;

int compute_amps() {
  s_batinfo bi;
  int dev = find_sensordev("acpiac0");
  if (read_sensor(&bi.ac, dev, SENSOR_INDICATOR, 0, "acpiac0.indicator0"))
    return 1;
  dev = find_sensordev("acpibat0");
  if (read_sensor(&bi.remaining, dev, SENSOR_AMPHOUR, 3, "acpibat0.amphour3") ||
      read_sensor(&bi.current, dev, SENSOR_AMPS, 0, "acpibat0.amps3") ||
      read_sensor(&bi.full, dev, SENSOR_AMPHOUR, 0, "acpibat0.apmhour0") ||
      read_sensor(&bi.low, dev, SENSOR_AMPHOUR, 2, "acpibat0.apmhour2"))
    return 1;
  int charge = (int)((bi.remaining - bi.low) * 100 / (bi.full - bi.low));
  int time_remaining = bi.current ?
    (bi.ac ?
     (int)((bi.full - bi.remaining) * 60 / bi.current) :
     (int)((bi.remaining - bi.low) * 60 / bi.current)) :
    0;
  if (!bi.ac)
    bi.current = -bi.current;
  printf("%3d%% %01.2fA %02d:%02d\n", charge,
	 bi.current / 1000000.0f,
	 time_remaining / 60, time_remaining % 60);
  return 0;
}

int compute_watts() {
  s_batinfo bi;
  int dev = find_sensordev("acpiac0");
  if (read_sensor(&bi.ac, dev, SENSOR_INDICATOR, 0, "acpiac0.indicator0"))
    return 1;
  dev = find_sensordev("acpibat0");
  if (read_sensor(&bi.remaining, dev, SENSOR_WATTHOUR, 3, "acpibat0.watthour3") ||
      read_sensor(&bi.current, dev, SENSOR_WATTS, 0, "acpibat0.power0") ||
      read_sensor(&bi.full, dev, SENSOR_WATTHOUR, 0, "acpibat0.watthour0") ||
      read_sensor(&bi.low, dev, SENSOR_WATTHOUR, 2, "acpibat0.watthour2"))
    return 1;
  int charge = (int)((bi.remaining - bi.low) * 100 / (bi.full - bi.low));
  int time_remaining = bi.current ?
    (bi.ac ?
     (int)((bi.full - bi.remaining) * 60 / bi.current) :
     (int)((bi.remaining - bi.low) * 60 / bi.current)) :
    0;
  if (!bi.ac)
    bi.current = -bi.current;
  printf("%3d%% %01.1fW %02d:%02d\n", charge,
	 bi.current / 1000000.0f,
	 time_remaining / 60, time_remaining % 60);
  return 0;
}

int main ()
{
  return (!compute_amps() || !compute_watts());
}
