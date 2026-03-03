build:
	cmake -B build
	cmake --build build
run: build
	./build/lab1/lab1
clean:
	rm -rf build
check:
	valgrind --leak-check=full --log-file=valgrind_report.txt ./build/lab1/lab1