#include "test_tftp_packet.h"
#include "test_tftp_io.h"

int main() {
	/* test tftp packet 
	test_guess_packet_type();
	test_buff_to_packet_read_write();*/ 
	test_buff_to_packet_data();
	test_buff_to_packet_ack();
	test_buff_to_packet_error();
	test_packet_data_to_bytes();
	test_packet_ack_to_bytes();
	test_packet_error_to_bytes();
	test_error_code();
	
	/* test tftp_io */
	test_write_block();
	test_read_block();
	return 0;
}