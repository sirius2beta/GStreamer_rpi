
#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>

#include <mosquitto.h>

typedef struct _CustomData {
  	GstElement *pipeline;
         /* Our one and only pipeline */
	
} CustomData;

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "test/t1", 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	CustomData* data = (CustomData*) obj;
	printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
	data->pipeline = gst_parse_launch("gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=-1 ! video/x-raw, width=640, height=480, framerate=12/1 ! videoconvert ! jpegenc ! rtpjpegpay ! udpsink host=10.8.0.4 port=5200",
      NULL);
	gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
}

int main(int argc, char *argv[]) {
  gst_init (&argc, &argv);
	int rc, id=12;
	CustomData data;

	mosquitto_lib_init();
	/* Initialize GStreamer */
  	gst_init (&argc, &argv);
	

	struct mosquitto *mosq;

	mosq = mosquitto_new("subscribe-test", true, &data);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	
	rc = mosquitto_connect(mosq, "192.168.0.104", 1883, 10);
	if(rc) {
		printf("Could not connect to Broker with return code %d\n", rc);
		return -1;
	}

	mosquitto_loop_start(mosq);
	printf("Press Enter to quit...\n");
	getchar();
	mosquitto_loop_stop(mosq, true);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	/* Free resources */
 	gst_element_set_state (data->pipeline, GST_STATE_NULL);
  	gst_object_unref (data->pipeline);


	return 0;
}
