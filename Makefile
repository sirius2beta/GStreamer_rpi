all: mqtt_pub mqtt_sub
	echo "Build finish!"

mqtt_pub: mqtt_pub.c
	gcc mqtt_pub.c -o mqtt_pub -lmosquitto

mqtt_sub: mos_subscriber.c
	gcc mqtt_sub.c -o mos_subscriber -lmosquitto `pkg-config --cflags --libs gstreamer-1.0`

clean:
	rm mqtt_pub mqtt_sub
