CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -I../tests/stubs -I..

PET_TEST_OBJS = build/pet_tests.o build/pet.o build/species_chirp.o build/thumb_format.o build/audio_stub.o build/natures.o
BATTLE_TEST_OBJS = build/battle_tests.o build/battle.o

.PHONY: test web-test clean

test: build/pet_tests build/battle_tests build/pokeball_tests build/nature_tests build/garden_tests build/move_tests build/gym_tests build/misc_tests build/progress_tests build/content_tests build/system_tests build/dex9_tests web-test
	./build/pet_tests
	./build/battle_tests
	./build/pokeball_tests
	./build/nature_tests
	./build/garden_tests
	./build/move_tests
	./build/gym_tests
	./build/misc_tests
	./build/progress_tests
	./build/content_tests
	./build/system_tests
	./build/dex9_tests

web-test:
	node web_serial_tests.mjs

build:
	mkdir -p build

build/pet_tests: $(PET_TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(PET_TEST_OBJS) -o $@

build/pet_tests.o: pet_tests.cpp ../pet.h ../dex.h | build
	$(CXX) $(CXXFLAGS) -c pet_tests.cpp -o $@

build/pet.o: ../pet.cpp ../pet.h ../dex.h ../audio.h ../natures.h | build
	$(CXX) $(CXXFLAGS) -c ../pet.cpp -o $@

build/natures.o: ../natures.cpp ../natures.h | build
	$(CXX) $(CXXFLAGS) -c ../natures.cpp -o $@

build/species_chirp.o: ../species_chirp.cpp ../species_chirp.h ../dex.h | build
	$(CXX) $(CXXFLAGS) -c ../species_chirp.cpp -o $@

build/thumb_format.o: ../thumb_format.cpp ../thumb_format.h | build
	$(CXX) $(CXXFLAGS) -c ../thumb_format.cpp -o $@

build/audio_stub.o: audio_stub.cpp ../audio.h | build
	$(CXX) $(CXXFLAGS) -c audio_stub.cpp -o $@

build/battle_tests: $(BATTLE_TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(BATTLE_TEST_OBJS) -o $@

build/battle_tests.o: battle_tests.cpp ../battle.h | build
	$(CXX) $(CXXFLAGS) -c battle_tests.cpp -o $@

build/battle.o: ../battle.cpp ../battle.h | build
	$(CXX) $(CXXFLAGS) -c ../battle.cpp -o $@

clean:
	rm -rf build

build/pokeball_tests: pokeball_tests.cpp ../pokeballs.cpp ../pokeballs.h ../dex.h | build
	$(CXX) $(CXXFLAGS) pokeball_tests.cpp ../pokeballs.cpp -o $@

build/nature_tests: nature_tests.cpp ../natures.cpp ../natures.h | build
	$(CXX) $(CXXFLAGS) nature_tests.cpp ../natures.cpp -o $@

build/garden_tests: garden_tests.cpp ../garden.cpp ../garden.h | build
	$(CXX) $(CXXFLAGS) garden_tests.cpp ../garden.cpp -o $@

build/move_tests: move_tests.cpp ../moves.cpp ../moves.h ../battle.cpp ../battle.h ../dex.h | build
	$(CXX) $(CXXFLAGS) move_tests.cpp ../moves.cpp ../battle.cpp -o $@

build/gym_tests: gym_tests.cpp ../gyms.cpp ../gyms.h ../dex.h | build
	$(CXX) $(CXXFLAGS) gym_tests.cpp ../gyms.cpp -o $@

MISC_SRC = ../expedition.cpp ../tradeevo.cpp ../regional.cpp ../gimmicks.cpp ../seasonal.cpp ../pokeballs.cpp ../garden.cpp
build/misc_tests: misc_tests.cpp $(MISC_SRC) | build
	$(CXX) $(CXXFLAGS) misc_tests.cpp $(MISC_SRC) -o $@

build/progress_tests: progress_tests.cpp ../dexbits.cpp ../egg.cpp ../steps.cpp | build
	$(CXX) $(CXXFLAGS) progress_tests.cpp ../dexbits.cpp ../egg.cpp ../steps.cpp -o $@

build/content_tests: content_tests.cpp ../achievements.cpp ../accessories.cpp ../quiz.cpp ../battle.cpp | build
	$(CXX) $(CXXFLAGS) content_tests.cpp ../achievements.cpp ../accessories.cpp ../quiz.cpp ../battle.cpp -o $@

SYS_SRC = ../party.cpp ../moves.cpp ../battle.cpp ../photo.cpp ../blenotify.cpp ../watchface.cpp
build/system_tests: system_tests.cpp $(SYS_SRC) | build
	$(CXX) $(CXXFLAGS) system_tests.cpp $(SYS_SRC) -o $@

build/dex9_tests: dex9_tests.cpp ../dexall.h ../dex_gen9.h ../gyms.cpp ../dex.h | build
	$(CXX) $(CXXFLAGS) dex9_tests.cpp ../gyms.cpp -o $@
