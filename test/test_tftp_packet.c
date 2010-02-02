#include "../tftp_packet.c"

void test_read_packet_read_write() {
	char packet[512+4];
	read_packet_read_write(char *buff, int bufflen, struct packet_read_write *paquet);
}