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
void fixSubject(string & subject);
void fixSpaces(string & str);

string getCourseURL(string str);

//TODO: Instead of writing to file, get string directly?
//TODO: fix no prerequisite after id (id after id before prerequisite)
int main(void)
{
    // used to denote if searching for class again or not
    bool searchAgain = true;

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
        fixSubject(subject);
        // file handling
        CURL *curl_handle;
        static const char *headerfilename = "head.txt";
        FILE *headerfile;

        if ( subject.compare("BICD") == 0 ) {
            subject = "BIOL";
        }


        string endurl = ".html";

        string begurl = "http://www.ucsd.edu/catalog/courses/";

        string urlStr = begurl + subject + endurl; 

        curl_global_init(CURL_GLOBAL_ALL);

        /* init the curl session */
        curl_handle = curl_easy_init();

        /* set URL to get */
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

        string prereqStr = "Prerequisites:";
        string prereqStrend = "</strong>";
        string endSubStr = "</p>";
        string idStr = "a id=";

        // find first occurence of "Prerequisites:" after class name
        int fixprereqPos = prereqSubStr.find(prereqStr);

        // find "a id="

        int idPos = prereqSubStr.find(idStr);

        if ( idPos < fixprereqPos ) {
            cout << "none." << endl;
        }
        else {

            // fix situation of "Prerequisites: "
            if ( fixprereqPos != -1 && (prereqSubStr[fixprereqPos+1]) == ' ' ) {
                cout << "Fixed\n";
                prereqStr += " ";
            }

            int prereqPos = prereqSubStr.find(prereqStr);

            // get substring between "Prerequisites:" and "</p>"
            int endPos = prereqSubStr.substr(prereqPos + prereqStr.size() + prereqStrend.size()).find(endSubStr); 

            // fix substring to trim edges -- make neater
            string newStr = prereqSubStr.substr(prereqPos + prereqStr.size() + prereqStrend.size() + 1, endPos - 1);

            //fixSpaces(newStr);

            ofstream out ("stuff", ofstream::out);

            out << newStr;

            cout << newStr << endl;
        }

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

void fixSubject(string & subject) {
    string swapBIOL[11] = {"BILD", "BIBC", "BICD", "BIEB", "BIMM", "BIPN", "BISP", "BGGN", "BGJC", "BGRD", "BGSE"};
    string swapNANO = "CENG";
    string swapLING = "LI";
    string swapHIST = "HI";

    if ( !subject.substr(0,2).compare("LI") ) { 
        subject = "LING";
        return;
    }
    if ( !subject.substr(0,2).compare(swapHIST) ) {
        subject = "HIST";
        return;
    }

    if ( !subject.compare(swapNANO) ) {
        subject = "NANO";
        return;
    }
    for ( int i = 0; i < 11; i++ ) {
        if ( !subject.compare(swapBIOL[i]) )
            subject = "BIOL";
    }
}


void fixSpaces(string & str) {

    string end = "";
    string beg = "";

    for ( int i = 0; i < str.length() - 1; i++ ) {
        // cut out from middle of string
        if( (str[i] == ' ' || str[i] == 9 || str[i] == 8) && 
            (str[i+1] == ' ' || str[i+1] == 9 || str[i+1] == 8 )) {
            end = str.substr(str.find(str[i+2]));
            beg = str.substr(0, 2);
            str = beg + end;
        }
    }
    str = beg + " " + end;
}

