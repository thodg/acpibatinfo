#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/sensors.h>

int sysctl_acpibat ()
{
  int mib[5] = { CTL_HW, HW_SENSORS, 0, SENSOR_AMPHOUR, 0 };
  size_t len;
  struct sensor s;

  if (sysctl(mib, 5, NULL, &len, NULL, 0) == -1)
    err(1, "sysctl");
  if (len != sizeof(s))
    err(1, "incompatible size for struct sensor: %d instead of %d",
	len, sizeof(s));
  if (sysctl(mib, 5, &s, &len, NULL, 0) == -1)
    err(1, "sysctl");

}

int main ()
{
  sysctl_acpibat();
  return 0;
}
