cmd_/home/pi/learn/Module/sensor_temp/sensor_temp.ko := ld -r  -EL -T ./scripts/module-common.lds -T ./arch/arm/kernel/module.lds  --build-id  -o /home/pi/learn/Module/sensor_temp/sensor_temp.ko /home/pi/learn/Module/sensor_temp/sensor_temp.o /home/pi/learn/Module/sensor_temp/sensor_temp.mod.o ;  true