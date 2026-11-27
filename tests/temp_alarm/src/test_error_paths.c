/*
 * temp_alarm - Error-path unit tests (homework skeleton)
 *
 * Task 2: fill in the stubs according to TEST_SPEC.md. Each stub calls
 * ztest_test_skip(), so the binary builds and runs cleanly until the test is
 * implemented.
 *
 * Run:
 *   west twister -T tests/temp_alarm -p native_sim
 */

#include <errno.h>
#include <zephyr/fff.h>
#include <zephyr/ztest.h>

#include "sensor_fake.h"
#include "temp_alarm.h"

#define DEFAULT_THRESHOLD 30

/* Same before() hook as in test_basic.c: a clean fake and an initialised module. */
static void before(void *fixture)
{
	ARG_UNUSED(fixture);

	sensor_fake_reset();
	sensor_fake_init();

	zassume_ok(temp_alarm_init(&fake_sensor_dev, DEFAULT_THRESHOLD),
		   "precondition: temp_alarm_init must succeed");
}

/*
 * ============================================================================
 * Test Suite: temp_alarm_sensor_errors
 *
 * Failures from the sensor driver, injected through the fakes.
 * ============================================================================
 */
ZTEST_SUITE(temp_alarm_sensor_errors, NULL, NULL, before, NULL, NULL);

ZTEST(temp_alarm_sensor_errors, test_fetch_error_propagates)
{
	/* (l8-task2): a failed fetch is reported,
	 * and the module does not go on to read the channel.
	 * See TEST_SPEC.md "Suite temp_alarm_sensor_errors" #1.
	 */
	// ztest_test_skip();
	struct temp_alarm_status status ; 

	fake_sensor_sample_fetch_fake.return_value  = -EIO ;

	zassert_equal(temp_alarm_check(void),-EIO, " must be -EIO for the fetch failure");

	zassert_ok(temp_alarm_get_status(&status),"must get_status succeed");

	zassert_equal(fake_sensor_channel_get_fake.call_count, 0 , "sesor channel get fake should not  be called, got %d", fake_sensor_channel_get_fake.call_count);

	zassert_equal(status.last_error, -EIO, " last_error must be -EIO because of fetch failure got %d",status.last_error);
}

ZTEST(temp_alarm_sensor_errors, test_channel_get_error_propagates)
{
	/* (l8-task2): a failed channel read is reported.
	 * See TEST_SPEC.md "Suite temp_alarm_sensor_errors" #2.
	 */
	// ztest_test_skip();

	RESET_FAKE(fake_sensor_channel_get) ;  // to remove the custome fake of fake_sensor_channel_get
	struct temp_alarm_status status ; 

	fake_sensor_channel_get_fake.return_value =  -EIO;

	zassert_equal(temp_alarm_check(void), -EIO, "must be -EIO for the channel get error"):


	zassert_ok(temp_alarm_get_status(&status),"must get_status succeed");

	zassert_equal(fake_sensor_sample_fetch_fake.call_count, 1 , "sesor fetch fake should be called once got %d", fake_sensor_sample_fetch_fake.call_count);

	zassert_equal(status.last_error, -EIO, " last_error must be -EIO because of channel get failure got %d",status.last_error);
}

ZTEST(temp_alarm_sensor_errors, test_fail_then_recover)
{
	/* (l8-task2): the first fetch fails and the second one succeeds.
	 * The module must recover on the second check.
	 * See TEST_SPEC.md "Suite temp_alarm_sensor_errors" #3.
	 */
	// ztest_test_skip();
	struct temp_alarm_status status ; 
	fake_sensor_sample_fetch_fake.return_value  = -EIO ;
	zassert_equal(temp_alarm_check(void),-EIO, " must be -EIO for the fetch failure");

	RESET_FAKE(fake_sensor_sample_fetch);  // to reset the fake_sensor_sample_fetch return value
	sensor_fake_set_temperature(20);

	zassert_ok(temp_alarm_check(void)," must be 0 for the fetch after RESET_FAKE of return value");

	zassert_ok(temp_alarm_get_status(&status),"must get_status succeed");

	zassert_equal(status.last_temp, 20 , "last_temp must be 20");

	zassert_false(status.is_alarming, "Is_alarming must be false");

	zassert_equal(status.last_error, -EIO, "the last error stored from fetch failure should be - EIO got %d",status.last_error);
}

/*
 * ============================================================================
 * Test Suite: temp_alarm_invalid_input
 *
 * Arguments the public API must reject.
 * ============================================================================
 */
ZTEST_SUITE(temp_alarm_invalid_input, NULL, NULL, before, NULL, NULL);

ZTEST(temp_alarm_invalid_input, test_get_status_null)
{
	/* (l8-task2): a NULL status pointer is rejected.
	 * See TEST_SPEC.md "Suite temp_alarm_invalid_input" #1.
	 */
	// ztest_test_skip();

	zassert_equal(temp_alarm_get_status(NULL), -EINVAL, "return should be -EINVAL for NULL");

}

ZTEST(temp_alarm_invalid_input, test_init_without_device)
{
	/* (l8-task2): init without a sensor device is rejected.
	 * See TEST_SPEC.md "Suite temp_alarm_invalid_input" #2.
	 */
	// ztest_test_skip();

	zassert_equal(temp_alarm_init(NULL,30),-EINVAL, "init must be NULL");

}

ZTEST(temp_alarm_invalid_input, test_init_threshold_out_of_range)
{
	/* (l8-task2): init rejects thresholds outside the allowed range.
	 * See TEST_SPEC.md "Suite temp_alarm_invalid_input" #3.
	 */
	// ztest_test_skip();
	zassert_equal(temp_alarm_init(&fake_sensor_dev,-1),-EINVAL, " must be -EINVAL because threshold is -1");
	zassert_equal(temp_alarm_init(&fake_sensor_dev,101),-EINVAL, " must be -EINVAL because threshold is 101");
}

ZTEST(temp_alarm_invalid_input, test_set_threshold_out_of_range)
{
	/* (l8-task2): set_threshold rejects values outside the allowed range,
	 * and keeps the old threshold.
	 * See TEST_SPEC.md "Suite temp_alarm_invalid_input" #4.
	 */
	// ztest_test_skip();
	zassert_equal(temp_alarm_set_threshold(-1),-EINVAL,"Must be -EINVAL because threshold for -1");
	zassert_equal(temp_alarm_set_threshold(101),-EINVAL,"Must be -EINVAL because threshold for 101");
	
}
