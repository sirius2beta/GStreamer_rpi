all: mqtt_pub mqtt_sub
	echo "Build finish!"

mqtt_pub: mqtt_pub.c
	gcc mqtt_pub.c -o mqtt_pub -lmosquitto

mqtt_sub: mos_client.c
	gcc mos_client.c -o mos_subscriber -lmosquitto `pkg-config --cflags --libs gstreamer-1.0`
mqtt_sub_cpp: mos_client.cpp
	gcc mos_client.cpp -o mos_subscriber -lmosquitto `pkg-config --cflags --libs gstreamer-1.0`

clean:
	rm mqtt_pub mqtt_sub
