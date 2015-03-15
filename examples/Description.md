Example use
======================================================

- start service; aquire socket
- create resource
- publish resource (somewhere)

```c++

Service servic = new Service("org.freedesktop.RFDS.Power");


QString status_path = "/sys/class/power_supply/BAT0/status"
QString energy_full_path = "/sys/class/power_supply/BAT0/energy_full"
QString energy_now_path = "/sys/class/power_supply/BAT0/energy_now"
Resource bat0;

bat[CHARGING_KEY] = true;
bat[ENERGY_FULL] = 51540000;
bat[ENERGY_NOW] = 3830000;

service->addResource(bat0, "/batteries/BAT0");

for(;;)
{
	bat0.beginUpdate();
	bat0->setValue("Charging", QString("Charging") == readAll(status_path));
	bat0->setValue("energy_full", readAll(energy_full_path).toInt());
	bat0->setValue("energy_now", readAll(energy_now_path).toInt());
	bat0.endUpdate();

	Thread::sleep(1);
}
```