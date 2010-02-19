#include "test_tftp_packet.h"

int main() {
	// test auxi
	// test tftp packet
	test_guess_packet_type();
	test_buff_to_packet_read_write();
	test_buff_to_packet_data();
	test_buff_to_packet_ack();
	test_buff_to_packet_error();
	test_packet_read_write_to_bytes();
	test_packet_data_to_bytes();
	test_packet_ack_to_bytes();
	test_packet_error_to_bytes();
	test_error_code();
	return 0;
}