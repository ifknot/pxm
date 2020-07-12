#include "catch2.h"

#include <iostream>

#include "spsc_concurrent_bounded_queue.h"

#define TEST_CAPACITY 31

using queue_t = pxm::spsc_concurrent_bounded_queue<int, TEST_CAPACITY>;

SCENARIO("spsc queue", "[queues]") {
	GIVEN("empty queue") {
		queue_t q;
		REQUIRE(q.size_guess() == 0);
		REQUIRE(q.capacity() == TEST_CAPACITY);
		REQUIRE(q.was_empty());
		REQUIRE_FALSE(q.was_full());
		WHEN("fill queue") {
			int i{ 0 };
			while (q.try_push(i++)) {}
			THEN("full queue") {
				REQUIRE(q.size_guess() == TEST_CAPACITY);
				REQUIRE(q.capacity() == TEST_CAPACITY);
				REQUIRE_FALSE(q.was_empty());
				REQUIRE(q.was_full());
			}
			WHEN("drain queue") {
				int i, j{ 0 };
				while (q.try_pop(i)) {
					REQUIRE(i == j++);
				}
				THEN("empty queue again") {
					//REQUIRE(i == TEST_CAPACITY);
					REQUIRE(q.size_guess() == 0);
					REQUIRE(q.capacity() == TEST_CAPACITY);
					REQUIRE(q.was_empty());
					REQUIRE_FALSE(q.was_full());
				}
			}
		}
		
	}
}

/*
SCENARIO("name", "[group]") {
	GIVEN() {
		REQUIRE();
		WHEN() {
			THEN() {
				REQUIRE();
			}
		}
	}
}
*/