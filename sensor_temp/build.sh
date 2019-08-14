#!/bin/bash
echo "clean"
	make clean
echo "rebuild"
	make all
echo "remove module"
	sudo rmmod sensor_temp
echo "install module"
	sudo insmod sensor_temp.ko

