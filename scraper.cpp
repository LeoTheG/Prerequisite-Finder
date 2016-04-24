#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

using namespace std;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

bool sayYes(string result); 

int main(void)
//int main(int argc, char * argv[])
{
    bool searchAgain = true;
    while ( searchAgain == true ) {
        CURL *curl_handle;
        static const char *headerfilename = "head.txt";
        FILE *headerfile;
        //static const char *bodyfilename = "body.txt";
        //FILE *bodyfile;

        string url = "http://www.ucsd.edu/catalog/courses/";

        curl_global_init(CURL_GLOBAL_ALL);

        /* init the curl session */
        curl_handle = curl_easy_init();

        /* set URL to get */
        //curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.ucsd.edu/catalog/curric/MATH-ug.html");
        curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.ucsd.edu/catalog/courses/MATH.html");


        /* no progress meter please */
        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

        /* send all data to this function  */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);

        /* open the files */
        headerfile = fopen(headerfilename,"w");
        if (headerfile == NULL) {
        curl_easy_cleanup(curl_handle);
        return -1;
        }
        /*
        bodyfile = fopen(bodyfilename,"w");
        if (bodyfile == NULL) {
        curl_easy_cleanup(curl_handle);
        return -1;
        }
        */

        /* we want the headers to this file handle */
        curl_easy_setopt(curl_handle,   CURLOPT_WRITEDATA, headerfile);

        /*
        * Notice here that if you want the actual data sent anywhere else but
        * stdout, you should consider using the CURLOPT_WRITEDATA option.  */

        /* get it! */
        curl_easy_perform(curl_handle);

        /* close the header file */
        fclose(headerfile);

        /* cleanup curl stuff */
        curl_easy_cleanup(curl_handle);

        /* place HTML into string */

        ifstream in;
        in.open(headerfilename);

        stringstream sstr;
        sstr << in.rdbuf();
        string str = sstr.str();

        /* -- CUSTOM CLASS SEARCH -- */

        string searchStr = "";

        cout << "Enter class name: ";
        cin >> searchStr;

        int pos = str.find(searchStr);

        //int pos = str.find(argv[1]);
        //int pos = str.find("math20c");
        
        string prereqSubStr = str.substr(pos);

        string prereqStr = "Prerequisites:</strong>";

        int prereqPos = prereqSubStr.find(prereqStr);

        //cout << prereqSubStr.substr(prereqPos + prereqStr.size()) << endl;
        string endSubStr = "</p>";
        int endPos = prereqSubStr.substr(prereqPos + prereqStr.size()).find(endSubStr); 

        string newStr = prereqSubStr.substr(prereqPos + prereqStr.size() + 1, endPos - 1);

        cout << newStr << endl;

        string result = "";

        cout << "Search again? (y/n): ";
        
        cin >> result;

        if ( sayYes(result) ) {
            searchAgain = true;
        }
        else {
            searchAgain = false;
        }

    }         
    return 0;
}

//TODO: fix magic numbers, add better searching for "yes"
bool sayYes(string result) {
    string yes[5] = {"y", "Y", "yes", "YES", "Yes"};
    
    for ( int i = 0; i < 5; i++ ) {
        if ( result.compare(yes[i]) == 0 )
            return true;
    }
    return false;
}
