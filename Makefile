scraper: scraper.o
	g++ -o scraper scraper.o -std=c++11 -lcurl
scraper.o: scraper.cpp 
	g++ -c -g scraper.cpp 
test: test.o
	g++ -o test test.o 
test.o: test.cpp
	g++ -c -g test.cpp 
clean:
	rm scraper *.o
