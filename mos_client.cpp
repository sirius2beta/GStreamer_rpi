
#include <iostream>
#include <gst/gst.h>

#include <mosquitto.h>

using namespace std;

typedef struct _CustomData {
  	GstElement *pipeline;
	bool streaming_started;
         /* Our one and only pipeline */
	
} CustomData;

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
	if(rc) {
		cout << "Error with result code:"<<rc<<endl;
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "test/t1", 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	CustomData* data = (CustomData*) obj;
	string raw_msg((char*)msg->payload);
	string cap;
	cout<<"New message with topic:"<<raw_msg<<endl;
	size_t space_pos = raw_msg.find(' ');
	if(space_pos == string::npos){
		cap = raw_msg;
	}else{
		cap = string(raw_msg,0, space_pos);
	}
	if(cap.compare(string("START")) == 0){
		if(data->streaming_started == false){
			data->pipeline = gst_parse_launch("gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=-1 ! video/x-raw, width=160, height=120, framerate=12/1 ! videoconvert ! jpegenc ! rtpjpegpay ! udpsink host=10.8.0.4 port=5200", NULL);
			gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
			data->streaming_started = true;
			cout<<"START..."<<endl;
		}else{
			gst_element_set_state (data.pipeline, GST_STATE_NULL);
  			gst_object_unref (data.pipeline);
			data->pipeline = gst_parse_launch("gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=-1 ! video/x-raw, width=640, height=480, framerate=12/1 ! videoconvert ! jpegenc ! rtpjpegpay ! udpsink host=10.8.0.4 port=5200", NULL);
			gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
		}
	}else if(cap.compare(string("GST")) == 0){
		string gst_command(raw_msg,space_pos+1,raw_msg.length()-space_pos-1);
		data->pipeline = gst_parse_launch(gst_command.c_str(), NULL);
		gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
		data->streaming_started = true;
		cout<<"GST_COMMAND : "<<gst_command<<endl;
	
	}else if(cap.compare(string("QUIT")) == 0){
		cout<<"quit..."<<endl;
	}else{
		cout<<"No matching cmd:"<<cap<<endl;
	}
}

int main(int argc, char *argv[]) {
  gst_init (&argc, &argv);
	int rc, id=12;
	
	CustomData data;
	
	data.streaming_started=false;

	mosquitto_lib_init();
	/* Initialize GStreamer */
  	gst_init (&argc, &argv);
	

	struct mosquitto *mosq;

	mosq = mosquitto_new("subscribe-test", true, &data);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	
	rc = mosquitto_connect(mosq, "192.168.0.104", 1883, 10);
	if(rc) {
		cout<<"Could not connect to Broker with return code %d\n"<<rc<<endl;
		return -1;
	}

	mosquitto_loop_start(mosq);
	cout<<"Press Enter to quit...\n";
	getchar();
	mosquitto_loop_stop(mosq, true);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	/* Free resources */
	if(data.streaming_started){
 		gst_element_set_state (data.pipeline, GST_STATE_NULL);
  		gst_object_unref (data.pipeline);
	}


	return 0;
}
