# test/Makefile
CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra

test: test_time_to_words
	./test_time_to_words

test_time_to_words: test_time_to_words.cpp ../word-clock/time_to_words.cpp ../word-clock/time_to_words.h ../word-clock/config.h
	$(CXX) $(CXXFLAGS) -o $@ test_time_to_words.cpp

clean:
	rm -f test_time_to_words

.PHONY: test clean
