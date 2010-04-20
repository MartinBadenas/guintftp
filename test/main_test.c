#include "test_tftp_packet.h"
#include "test_tftp_io.h"
#include "test_tftp_mode.h"
#include "../tftp_log.h"

int main() {
	/* test tftp packet */
	/*log_info("tftp packet start");
	test_guess_packet_type();
	test_buff_to_packet_read_write();
	test_buff_to_packet_data();
	test_buff_to_packet_ack();
	test_buff_to_packet_error();
	test_packet_data_to_bytes();
	test_packet_ack_to_bytes();
	test_packet_error_to_bytes();
	test_error_code();
	log_info("tftp packet done!");*/
	
	/* test tftp_io */
	/*log_info("tftp io start");
	test_write_block();
	test_read_block();
	log_info("tftp io done!");*/
	
	/* test tftp_mode */
	text_move_right_insert();
	text_move_right_put_after();
	test_mode_to_chars();
	return 0;
}