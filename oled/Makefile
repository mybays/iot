all:rpi

rpi:ssd1329_rpi ssd1332_rpi


ssd1329_rpi:
	gcc -o ssd1329_rpi ssd1329_rpi.c ssd1329.c -lwiringPi
ssd1332_rpi:
	gcc -o ssd1332_rpi ssd1332_rpi.c ssd1332.c -lwiringPi

clean:
	rm -fr ssd1329_rpi ssd1332_rpi