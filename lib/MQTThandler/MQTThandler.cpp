// Neil Flynn
// 7-22-2018
// V1

#include "MQTThandler.h"

// MQTThandler
// wrapper class for the pub sub client class, use to hide callback from main.
// Will add some of my own code in here to extend pub sub client

void MQTThandler::callback(char* topic, uint8_t* payload, unsigned int length)
{
	// remove the serial debug
	/*
	Serial.print("C Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	*/
	char* somearray = new char[length + 1];
	for (unsigned int i = 0; i < length; i++) {
		somearray[i] = ((char)payload[i]);
	}
	somearray[length] = '\0'; // null termination
	Inc_message = somearray;
	mailFlag = true;

	// remove the serial debug
	// Serial.println("C message is " + Inc_message);
}

void MQTThandler::CBbinMsg(char* topic, uint8_t* payload, unsigned int length)
{
	
	for (unsigned int i = 0; i < length; i++) {
		B_message[i] = payload[i];
	}
	mailFlag = true;
}

// private function called to maintain the server connection
void MQTThandler::reconnect(){
	CurTime = millis();
	while ((!MQTTClient.connected())&&(pastTime < (CurTime - waitTime))){
		Serial.print("C Attempting MQTT connection...");
		// Attempt to connect
		if (MQTTClient.connect(ClientName.c_str())) {
			Serial.println("connected");
			MQTTClient.subscribe(InC_topic.c_str());
		}
		else {
			Serial.print("C failed, rc=");
			Serial.print(MQTTClient.state());
			Serial.println("C try again in 5 seconds");
			// Wait 5 seconds before retrying
			pastTime = CurTime;
		}
	}
}
//Constructor; need ESP wifi client pointer and broker address
MQTThandler::MQTThandler(Client& _ClWifi, const char* _serverName){
	ClWifi = &_ClWifi;
	MQTTClient.setClient(_ClWifi);
	MQTTClient.setServer(_serverName, 1883);
	MQTTClient.setCallback([this](char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
	mode = 0;
	mailFlag = false;
}
//Constructor; need ESP wifi client pointer and broker address
MQTThandler::MQTThandler(Client& _ClWifi, IPAddress _brokerIP){
	ClWifi = &_ClWifi;
	brokerIP = _brokerIP;
	MQTTClient.setClient(_ClWifi);
	MQTTClient.setServer(_brokerIP, 1883);
	MQTTClient.setCallback([this](char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
	mode = 0;
	mailFlag = false;
}

//Constructor for binary payload; need ESP wifi client pointer and broker address
MQTThandler::MQTThandler(Client & _ClWifi, IPAddress _brokerIP, uint8_t _mode, uint _bufferSz)
{
	ClWifi = &_ClWifi;
	brokerIP = _brokerIP;
	MQTTClient.setClient(_ClWifi);
	MQTTClient.setServer(_brokerIP, 1883);
	mode = _mode;
	bufferSz = _bufferSz;
	if (_mode = 0)
		MQTTClient.setCallback([this](char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
	else {
		MQTTClient.setCallback([this](char* topic, byte* payload, unsigned int length) { this->CBbinMsg(topic, payload, length); });
		B_message = new byte[bufferSz];
	}
	mailFlag = false;
}
// update will return true if a message has been recieved
int MQTThandler::update(){
	if(!MQTTClient.connected())
		reconnect();
	MQTTClient.loop();
	return mailFlag;
}

// set client Name
void MQTThandler::setClientName(String _clientName){
	ClientName = _clientName;
}

// set topic for incomming messages
void MQTThandler::subscribeIncomming(String topic){
	InC_topic = topic;
}

// set topic for outgoing messages
void MQTThandler::subscribeOutgoing(String topic){
	Out_topic = topic;
}

// publish an outgoing message
int MQTThandler::publish(String message){
	if (!MQTTClient.connected()) {
		reconnect();
	}
	MQTTClient.publish(Out_topic.c_str(),message.c_str());
	return 0;
}

// return any recived messages
String MQTThandler::GetMsg(){
	String RetVal;
	if (mailFlag){
		RetVal = Inc_message;
		mailFlag = false;
	}
	else
		RetVal = "";
	return RetVal;
}
