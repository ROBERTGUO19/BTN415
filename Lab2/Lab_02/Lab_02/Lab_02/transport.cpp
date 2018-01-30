#include "transport.h"

udp_client::udp_client(int port) {
	this->port = port;
}

udp_server::udp_server(int port, string data) {
	this->port = port;
	this->data = data;
	this->current_index = 0;
	this->data_size = data.size();
}

udp_packet udp_node::send_packet(int client_port) {
	//sends a packet with one character at a time
	udp_packet packet;
	packet.source_port = this->port;
	packet.dest_port = client_port;
	packet.data = this->data.substr(this->current_index, 1);
	return packet;
}

void udp_node::request_transmission(udp_node *transmitter, channel * wireless){
	while (transmitter->get_current_index() < transmitter->get_data_size()) {
		this->rec = wireless->transmit(transmitter->send_packet(this->port));
		this->data += this->rec.data;
		transmitter->update_current_index();
	}
	cout << "Data received." << endl;
}

channel::channel(int prob_lost_data, int prob_corrupt_data) {
	srand(time(NULL)); //initializes a seed for the rand function
	this->prob_lost_data = prob_lost_data;
	this->prob_corrupt_data = prob_corrupt_data;
}

//sends the data over the channel. Switches data to empty or to # in case the 
//data was lost or currupted, respectively
udp_packet channel::transmit(udp_packet packet) {
	int random = rand() % 100;
	if (random > this->prob_lost_data) {
		random = rand() % 100;
		if (random > this->prob_corrupt_data) {
			return packet; //data wasn't lost or corrupted
		}
		else {
			packet.data = "#"; //data was corrupted
			return packet;
		}
	}
	else {
		packet.data = "";//data was lost
		return packet;
	}
}

//void tcp_node::send_signal(tcp_node *dest, tcp_node *transmitter, channel *wireless, string data) {
//    tcp_packet packet;
//    packet.source_port = transmitter->port;
//    packet.dest_port = dest->port;
//    packet.acknowledge = true;
//    packet.checksum = true;
//    packet.data = data;
//
//    dest->rec.checksum = false;
//    dest->rec.acknowledge = false;
//    while (not dest->rec.checksum or not dest->rec.acknowledge) {
//        dest->rec = wireless->transmit(packet);
//    }
//}
