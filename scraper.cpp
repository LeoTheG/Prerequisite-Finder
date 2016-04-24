#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <cstring>

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
bool tryAgain(string & result, bool & searchAgain); 

string getCourseURL(string str);

//TODO: Instead of writing to file, get string directly?
int main(void)
{
    // used to denote if searching for class again or not
    bool searchAgain = true;

    locale loc;

    // search loop
    while ( searchAgain == true ) {
        
        // get class name
        string searchStr = "";
        // get subject (MATH, ANTH, etc.) - derived from class name
        string subject = "";
        string result = "";

        cout << "Enter class name: ";
        cin >> searchStr;

        subject = getCourseURL(searchStr);
        for ( int i = 0; i < subject.length(); i++ ) {
            subject[i] = toupper(subject[i]);
        }
        // file handling
        CURL *curl_handle;
        static const char *headerfilename = "head.txt";
        FILE *headerfile;

        string endurl = ".html";

        string begurl = "http://www.ucsd.edu/catalog/courses/";

        string urlStr = begurl + subject + endurl; 

        curl_global_init(CURL_GLOBAL_ALL);

        /* init the curl session */
        curl_handle = curl_easy_init();

        /* set URL to get */
        //curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.ucsd.edu/catalog/courses/MATH.html");
        curl_easy_setopt(curl_handle, CURLOPT_URL, urlStr.c_str());

        /* no progress meter please */
        //curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

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

        // handle page not found error
        if( str.find("<title>UC San Diego : File Not Found (404)</title>") != -1 ) {
            cout << "Invalid subject\n";
            tryAgain(result,searchAgain);
            if ( searchAgain ) continue;
            return -1;
        }

        /* -- CUSTOM CLASS SEARCH -- */

        int pos = str.find(searchStr);

        // handle class not found error
        if ( pos == -1 ) {
            cout << "Invalid class\n";
            tryAgain(result,searchAgain);
            if ( searchAgain ) continue;
        }

        string prereqSubStr = str.substr(pos);

        string prereqStr = "Prerequisites:</strong>";

        int prereqPos = prereqSubStr.find(prereqStr);

        //cout << prereqSubStr.substr(prereqPos + prereqStr.size()) << endl;
        string endSubStr = "</p>";
        int endPos = prereqSubStr.substr(prereqPos + prereqStr.size()).find(endSubStr); 

        string newStr = prereqSubStr.substr(prereqPos + prereqStr.size() + 1, endPos - 1);

        cout << newStr << endl;

        tryAgain(result,searchAgain);

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

string getCourseURL(string str) {

    // get string before first number
    int i = 0;
    while ( isalpha(str[i]) ) {
        i++;
    }
    return str.substr(0,i);
}

bool tryAgain(string & result, bool & searchAgain) {

    cout << "Search again? (y/n): ";
    cin >> result;

    if ( sayYes(result) ) {
        searchAgain = true;
    }
    else {
        searchAgain = false;
    }

}
