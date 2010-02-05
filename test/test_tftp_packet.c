#include "../tftp_packet.c"

void test_buff_to_packet_read_write() {
	char packet[512+4];
	buff_to_packet_read_write(char *buff, int bufflen, struct packet_read_write *paquet);
}