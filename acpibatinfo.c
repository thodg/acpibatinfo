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

  while (1) {
    if (sysctl(mib, 3, NULL, &len, NULL, 0) == -1)
      err(1, "sysctl");
    if (len != sizeof(d))
      err(1, "incompatible size for struct sensordev: %lu instead of %lu",
	  len, sizeof(d));
    if (sysctl(mib, 3, &d, &len, NULL, 0) == -1)
      err(1, "sysctl");
    if (strcmp(name, d.xname) == 0)
      return mib[2];
    mib[2]++;
  }
  return 0;
}

int64_t read_sensor (int dev, int type, int sensor)
{
  int mib[5] = { CTL_HW, HW_SENSORS, dev, type, sensor };
  size_t len;
  struct sensor s;

  if (sysctl(mib, 5, NULL, &len, NULL, 0) == -1)
    err(1, "sysctl");
  if (len != sizeof(s))
    err(1, "incompatible size for struct sensor: %lu instead of %lu",
	len, sizeof(s));
  if (sysctl(mib, 5, &s, &len, NULL, 0) == -1)
    err(1, "sysctl");
  //printf("%9llu %s\n", s.value, s.desc);
  return s.value;
}

int main ()
{
  int dev = find_sensordev("acpiac0");
  int64_t ac = read_sensor(dev, SENSOR_INDICATOR, 0);
  dev = find_sensordev("acpibat0");
  int64_t remaining = read_sensor(dev, SENSOR_AMPHOUR, 3);
  int64_t current = read_sensor(dev, SENSOR_AMPS, 0);
  int64_t full = read_sensor(dev, SENSOR_AMPHOUR, 0);
  int64_t low = read_sensor(dev, SENSOR_AMPHOUR, 2);
  int charge = (int)((remaining - low) * 100 / (full - low));
  int time_remaining = ac ?
    (int)((full - remaining) * 60 / current) :
    (int)((remaining - low) * 60 / current);
  if (!ac)
    current = -current;
  printf("%3d%% %01.2fA %02d:%02d\n", charge,
	 current / 1000000.0f,
	 time_remaining / 60, time_remaining % 60);
  return 0;
}
