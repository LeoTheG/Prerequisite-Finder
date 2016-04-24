scraper: scraper.o
	g++ -o scraper scraper.o -lcurl
scraper.o: scraper.cpp 
	g++ -c -g scraper.cpp 
clean:
	rm scraper *.o
