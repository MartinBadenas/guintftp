#ifndef TEST_TFTP_PACKET_H_
#define TEST_TFTP_PACKET_H_
#include "../tftp_packet.h"

void test_guess_packet_type();

void test_get_packet_read_write(packet_read_write *packet, char *mode);

void test_buff_to_packet_read_write();

void test_buff_to_packet_data();

void test_buff_to_packet_ack();

void test_buff_to_packet_error();

void test_packet_data_to_bytes();

void test_packet_ack_to_bytes();

void test_packet_error_to_bytes();

void test_error_code();

#endif /*TEST_TFTP_PACKET_H_*/
