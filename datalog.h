#ifndef DATALOG_H
#define DATALOG_H
#include <SPI.h>
#include <SD.h>
#include "error.h"

constexpr int SAMPLE_LIMIT = 6000;          // Number of samples before blinking LED
constexpr int SD_OK = 0;
constexpr int SD_ERR_INIT = 1;
constexpr int SD_ERR_NO_SPACE = 2;
constexpr int FILENAME_LENGTH = 12; // 8.3 name standard I guess

constexpr int SUCCESS = 0;
constexpr int WRITE_ERR =1;
constexpr int MAX_SAMPLE_COUNT_REACHED = 2;
extern int sample_counter;                // Counter to track written samples
extern File output_file;

template <typename T> struct indexed_element {

  T element;
  int index;
};

char constexpr MAX_PATH_LENGTH = 20;

void printDirectory(File dir, int numTabs, int depth = 0, int max_depth = 0);

// struct SDCard {

//   Sd2Card _card;
//   SdVolume _volume;
//   SdFile _sd_root;

//   public:
//     static SDCard& getInstance(const int chipSelect)
//     {
//         static SDCard instance; // Guaranteed to be destroyed.
//         SDCard::_SD_init(chipSelect);
//         return instance;
//     }
//     SDCard(SDCard const&)          = delete;
//     void operator=(SDCard const&)  = delete;
    
//     private:
//         SDCard() {}                    // Constructor? (the {} brackets) are needed here.
//         int _SD_init(const int chipSelect);

// };

class DataFile
/*
  This class represent the inderface for a data file with name
  "data%04d.csv", where the numeric part is the index of the file
  with prepended zero padding. 
  This interface allow for opening and closing the file, and for 
  the insertion of formatted data. However, the factory class for this
  one is the DataFileManager, that overviewes all the data file that are
  saved on the SD card. 
*/
{
  friend class DataFileManager;

  int _index;
  char full_path[MAX_PATH_LENGTH];
  File _file;

  public:
  DataFile(const int index, const char* path = "", bool append = false) : _index(index) {
    
    // Serial.println("Dentro il costrutture");

    char filename[13];
    sprintf(&filename[0], "DATA%04d.csv", _index);
    // Serial.println(filename);
    if (strlen(path)){
      snprintf(full_path, MAX_PATH_LENGTH, "%s/%s", path, filename);
    }
    else {
      snprintf(full_path, MAX_PATH_LENGTH, "%s", filename);
    }
    // Serial.println(full_path);
    // Serial.println("Breakpoint A ");
    // check if the file exists
    if (! append) {
      // Serial.println("Breakpoint D ");
      if (SD.exists(filename)) {
        SD.remove(filename);
      }
    }
    // Serial.println("Breakpoint B");
    _file = SD.open(full_path, FILE_WRITE);

    // Serial.println("Breakpoint C");
  }

  ~DataFile(){
    _file.close();
  }
};

class DataFileManager {

  friend class TestDataFileManager;
  
  File *_root = nullptr;
  static const int _capacity = 100;

  int _index = -1;

  void _find_next_index(const File &dir);
  static int _compare_ints(const void *arg1, const void *arg2){
    return *(int *)arg1 - *(int *)arg2;
  }

  public:
  

  DataFileManager(const char* dirname, const int chipSelect, const int error_pin){

    char name[13] = "/";
    if (strlen(dirname) != 0){
      strcpy(name, dirname);
    }
    
    // int code = SD_init(chipSelect);
    // if (code != SD_OK) {
    //   Serial.println("SD card initialization failed!");
    //   error(SD_ERROR, error_pin, false); // on error and blink forever and stop main program
    // }
    // _root = new File(_sd_root, name);
    // _find_next_index(*_root);
  };

  ~DataFileManager(){
    delete _root;
  };

  
  int save_data(const float* piezoValues, const int num_values, const int led_pin); // TODO: make it generic with a String formatter
};


// struct StringList {
//   /*
//   Static string list of size 100. I hack my file list in this way to avoid
//   using dynamic allocation in this RAM constrained environment. 

//   capacity: is the maximum size of the list. If the list becomes full, the next element 
//   to be pushed will be on top of the first 
//   */
//   static const unsigned int _capacity = 100;
//   unsigned int _size = 0;
//   //char* string_array[_capacity];
//   char string_data[_capacity][FILENAME_LENGTH+1];
//   char (*last_element)[FILENAME_LENGTH+1] = &string_data[0];
//   unsigned int _offset = 0;

//   StringList();
//   ~StringList();
  
//   virtual void push(const char* str){
//     last_element = &string_data[0] + (_size % _capacity);
//     strcpy(*(last_element++), str);
//     ++_size;
//   };

//   void pop() 
//   /*
//   If the size of the list, computed with modulo, goes under zero, 
//   _size is resetted to zero and last_element to the first slot of
//   string_data. 
//   */
//   {
//     if (_size > 0) {
//       last_element--;
//       _size--;
//     }
//   };
//   int size() {return min(++_size, _capacity);};

//   virtual void ls(File dir);
// };

// struct DataFileList : public StringList {

//   static constexpr int _index_length = 4; // the length of the string index in the file i.e. %04d
//   unsigned int index; // current index of the datafiles
  
//   File root;

//   DataFileList(const char* root_name)
//   /*
//     This constructor does the following things:
//     1. opens the given root directory (checks if is a directory)
//     2. build a list of all the files starting with the substring "DATA"
//     3. extract from each of them the index part, if any (there should be...), convert it to nubmer and store in a list
//     4. Sort the list, keeping  the index, and gets the highest index value
//     5. Initialize the sorted string list into the DataFileList and sets the index pointing to the highest index
//   */
//   {
//     index = 0;
//     // read file list and add to datafilelist all the files that start with DATA, then order them. 
//     root = SD.open(root_name); // TODO check for errors
//     DataFileList::ls(root);
    
//   }
  
//   void new_file(){
//     char str[FILENAME_LENGTH+1];
//     sprintf(str, "DATA%04u.csv", ++index);
//     File file = SD.open(str, FILE_WRITE);
//     if (file) StringList::push(str); //use this since index have been already incremented
//     file.close();
//   }

//   void push(const char* str){
//     StringList::push(str);
//     index++;
//   }
//   void ls(File dir);
// };
// extern StringList DATA_FILE_LIST;

// int SD_init(const int chipSelect);
String format_data(const float* piezoValues, const int len);

#endif