/*
 * Copyright (C) 2010 Dani Hernández Juárez, dhernandez0@gmail.com
 * Copyright (C) 2010 Martín Badenas Beltrán, martin.badenas@gmail.com
 *
 * This file is part of Guintftp.
 *
 * Guintftp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Guintftp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Guintftp.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../config.h"

/* Check-based unit tests */
#include <check.h>

#include "test_tftp_packet.c"
#include "test_tftp_io.c"
#include "test_tftp_mode.c"


Suite *guintftp_suite() {
	Suite *s = suite_create("guintftp");
	TCase *tc;

	/* test IO */
	tc = tcase_create("test io");

	tcase_add_test(tc, test_read_block);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_write_block);
	suite_add_tcase(s, tc);
	/* end test IO */

	/* test mode */
	tc = tcase_create("test mode");

	tcase_add_test(tc, test_move_right_insert);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_move_right_put_after);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_mode_to_chars);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_delete_character);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_chars_to_mode);
	suite_add_tcase(s, tc);
	/* end test mode */

	/* test packet */
	tc = tcase_create("test packet");

	tcase_add_test(tc, test_guess_packet_type);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_buff_to_packet_read_write);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_buff_to_packet_data);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_buff_to_packet_ack);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_buff_to_packet_error);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_packet_data_to_bytes);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_packet_ack_to_bytes);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_packet_error_to_bytes);
	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_error_code);
	suite_add_tcase(s, tc);
	/* end test packet */

	return s;
}

int main() {
	int nf;
	Suite *s = guintftp_suite();
	SRunner *sr = srunner_create(s);
	srunner_set_log(sr, "check_guintftp.log");
	srunner_run_all(sr, CK_NORMAL);
	nf = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (nf == 0) ? 0 : 1;
}
