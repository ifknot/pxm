#include "catch2.h"

#include <iostream>
#include <thread>

#include "spsc_concurrent_bounded_queue.h"

#define TEST_CAPACITY 31

using queue_t = pxm::spsc_concurrent_bounded_queue<int, TEST_CAPACITY>;

SCENARIO("spsc queue", "[pxm]") {
	GIVEN("a queue") {
		queue_t q;
		THEN("empty queue") {
			REQUIRE(q.size_guess() == 0);
			REQUIRE(q.capacity() == TEST_CAPACITY);
			REQUIRE(q.was_empty());
			REQUIRE_FALSE(q.was_full());
		}
		WHEN("fill queue") {
			int i{ 1 };
			while (q.try_push(i++)) {}
			THEN("full queue") {
				REQUIRE(q.size_guess() == TEST_CAPACITY);
				REQUIRE(q.capacity() == TEST_CAPACITY);
				REQUIRE_FALSE(q.was_empty());
				REQUIRE(q.was_full());
			}
			WHEN("drain queue") {
				int i, j{ 1 };
				while (q.try_pop(i)) {
					REQUIRE(i == j++);
				}
				THEN("empty queue again") {
					REQUIRE(i == TEST_CAPACITY);
					REQUIRE(q.size_guess() == 0);
					REQUIRE(q.capacity() == TEST_CAPACITY);
					REQUIRE(q.was_empty());
					REQUIRE_FALSE(q.was_full());
				}
			}
		}
		WHEN("refilled queue") {
			int i{ 1 };
			while (q.try_push(i++)) {}
			THEN("full queue") {
				REQUIRE(q.size_guess() == TEST_CAPACITY);
				REQUIRE(q.capacity() == TEST_CAPACITY);
				REQUIRE_FALSE(q.was_empty());
				REQUIRE(q.was_full());
			}
			WHEN("clear queue") {
				REQUIRE(q.abandon_all() == TEST_CAPACITY);
				THEN("empty queue") {
					REQUIRE(q.size_guess() == 0);
					REQUIRE(q.capacity() == TEST_CAPACITY);
					REQUIRE(q.was_empty());
					REQUIRE_FALSE(q.was_full());
					REQUIRE(q.abandon_all() == 0);
				}
			}
		}
		GIVEN("producer and consumer") {
			static int sample_size{ TEST_CAPACITY };
			std::atomic_bool stop{ false };
			int observed_sum{ 0 };

			auto producer = std::thread([&q, & stop]() {
				for (int i{ 1 }; i <= sample_size; ++i) {
					while (!q.try_push(i)) {}
				}
				stop.store(true);
			});
			
			auto consumer = std::thread([&q, &observed_sum, &stop]() {
				int i;
				while (!stop || !q.was_empty()) {
					q.try_pop(i);
					observed_sum += i;
				}
			}); 

			producer.join();
			consumer.join();
			int expected_sum = (sample_size * (sample_size + 1)) / 2;
			REQUIRE(observed_sum == expected_sum);
			REQUIRE(q.size_guess() == 0);
			REQUIRE(q.capacity() == TEST_CAPACITY);
			REQUIRE(q.was_empty());
			REQUIRE_FALSE(q.was_full());
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