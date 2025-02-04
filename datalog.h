#ifndef DATALOG_H
#define DATALOG_H
#include <SPI.h>
#include <SD.h>

constexpr int SAMPLE_LIMIT = 6000;          // Number of samples before blinking LED
constexpr int SD_OK = 0;
constexpr int SD_ERR_INIT = 1;
constexpr int SD_ERR_NO_SPACE = 2;
constexpr int FILENAME_LENGHT = 12; // 8.3 name standard I guess

constexpr int SUCCESS = 0;
constexpr int WRITE_ERR =1;
constexpr int MAX_SAMPLE_COUNT_REACHED = 2;
extern int sample_counter;                // Counter to track written samples
extern File output_file;


struct StringList {
  /*
  Static string list of size 100. I hack my file list in this way to avoid
  using dynamic allocation in this RAM constrained environment. 

  capacity: is the maximum size of the list. If the list becomes full, the next element 
  to be pushed will be on topo ov the first 
  */
  static const int _capacity = 100;
  unsigned int _size = 0;
  char* last_element = NULL;
  char* string_array[_capacity];
  char string_data[_capacity][FILENAME_LENGHT+1];

  StringList();
  ~StringList();
  virtual void push(const char* str){
    if (_size < _capacity) {
        strcpy((++last_element), str);
        _size++; 
    }
  };
  void pop() {
    if (_size > 0) {
      last_element--;
      _size--;
    }
  };
  int size() {return _size;};

  virtual void ls(File dir);
};

struct DataFileList : public StringList {
  unsigned int index; // current index of the datafiles
  File root;

  DataFileList(const char* root_name){
    index = 0;
    root = SD.open(root_name); // TODO check for errors
    // read file list and add to datafilelist all the files that start with DATA, then order them. 
    //strcpy(index, "0000");
  }
  
  void new_file(){
    char str[FILENAME_LENGHT+1];
    sprintf(str, "DATA%04u.csv", ++index);
    File file = SD.open(str, FILE_WRITE);
    if (file) StringList::push(str); //use this since index have been already incremented
    file.close();
  }

  void push(const char* str){
    StringList::push(str);
    index++;
  }
  void ls(File dir);
};
//extern StringList DATA_FILE_LIST;

int SD_init(const int chipSelect);
String format_data(const float* piezoValues, const int len);
int save_data(const float* piezoValues, const int num_values, const int led_pin);
void printDirectory(File dir, int numTabs);

#endif