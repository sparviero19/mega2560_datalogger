#include <Arduino.h>
#include <time.h>
#include "datalog.h"

int sample_counter = 0;
File output_file;

String DataFileManager::format_data(const float* piezoValues, const int len){
  // Format the data into a CSV-style string
  String data = String(millis()) + ',';
  for (int i = 0; i < len; i++) {
    data += String(piezoValues[i]);
    if (i < len - 1) data += ",";
  }
  return data;
}

const char* DataFileManager::format_piezo_data(const float* piezoValues, const int len){
  String formatted_data = format_data(piezoValues, len);
  char* out = new char[len+1];
  strcpy(out, formatted_data.c_str());
  return out; 
}

void printDirectory(File dir, int numTabs, int depth, int max_depth) {

  if (depth > max_depth) return;

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print("....");
    }

    Serial.print(entry.name());

    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1, depth +1, max_depth);
    } else {
      // files have sizes, directories do not
      Serial.print("....");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


//////// StringList methods ///////////////////

// void StringList::ls(File dir){

//   while(true){
//     File entry = dir.openNextFile();
//     if (!entry){
//       break; //no more files
//     }
    
//     if (entry.isDirectory()) continue;

//     this->push(entry.name());
//   }
// }

//////// DataFileManager related code ///////////////////

void DataFileManager::_find_next_index(const File &dir) {

  //Serial.println("-----> Breakpoint 1");
  int numeric_indexes[_capacity];
  int count = 0;

  while(true){
    //Serial.println("-----> Breakpoint 2");
    File entry = dir.openNextFile();
    if (!entry){
      break; //no more files
    }
    //Serial.println("-----> Breakpoint 3");
    if (entry.isDirectory()) continue;
    
    if (strstr(entry.name(), "DATA")!=NULL) {
      //Serial.print("-----> Breakpoint 4  index here is ");
      // extract id and convert to int
      //Serial.println(atoi(entry.name()+5));
      numeric_indexes[count++] = atoi(entry.name()+5); // the substing starting at position 5 should be the index 
    }    
  }
  // let's sort the indexes first
  //Serial.print("-----> Breakpoint 5 count = ");
  //Serial.println(count);
  qsort(numeric_indexes, count, sizeof(int), _compare_ints);
  //Serial.println("-----> Breakpoint 6");
  _index = numeric_indexes[count-1];
  // Serial.print("Index is: ");
  // Serial.println(_index);
  // for(int i=0; i<count; i++){
  //   Serial.print(numeric_indexes[i]);
  //   Serial.print(' ');
  // }
  // Serial.println();
}

DataFile* DataFileManager::open(int index, bool append){
  DataFile* new_df = nullptr;
  if(index>0) {
    // open a specific file using DataFile interface
    new_df = new DataFile(index, _root.name(), append);
  }
  else {
    // open a new file with the next index
    if(_index < 9999) {
      new_df = new DataFile(++_index, _root.name(), false);
    }
    else {
      Serial.println("Maximum index limit reached!");
      Serial.print("current index is "); 
      Serial.println(_index);
    }
  }

  return new_df;
  
}


////////////////////////////////////////////////////////////////77
int SD_info(const int chipSelect) {

  Sd2Card _card;
  SdVolume _volume;
  SdFile _sd_root;
  Serial.println("--- Some info on the SD card ---");
  Serial.print("\n\n");

  if (!_card.init(SPI_HALF_SPEED, chipSelect)) return SD_ERR_INIT;

  // check for SD info
  Serial.println();
  Serial.print("Card type:         ");
  switch (_card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  if (!_volume.init(_card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }

  Serial.print("Clusters:          ");
  Serial.println(_volume.clusterCount());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(_volume.blocksPerCluster());
  Serial.print("Total Blocks:      ");
  Serial.println(_volume.blocksPerCluster() * _volume.clusterCount());
  Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(_volume.fatType(), DEC);
  volumesize = _volume.blocksPerCluster();   // clusters are collections of blocks
  volumesize *= _volume.clusterCount();      // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);
  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);

  // list all files in the card with date and size
  Serial.println("----------------------------------------------------------");
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  _sd_root.openRoot(_volume);
  _sd_root.ls(LS_R | LS_DATE | LS_SIZE);
  _sd_root.close();
  if (!SD.begin(chipSelect)) return SD_ERR_INIT;

  return SD_OK;
}






int DataFile::save_data(const float* piezoValues, const int num_values, const int led_pin) {
  // Open the file in append mode
  output_file = SD.open("data.csv", FILE_WRITE);
  if (output_file) {
    String data = format_data(piezoValues, num_values);
    output_file.println(data);
    output_file.close(); // Close the file to ensure data is saved
    Serial.println("Data saved: " + data);

    // Increment sample counter
    sample_counter++;
    if (sample_counter >= SAMPLE_LIMIT) {
      three_blinks(led_pin);
      return MAX_SAMPLE_COUNT_REACHED;
    }
  return SUCCESS;
  } else {
    Serial.println("Error opening file for writing!");
    return WRITE_ERR;
  }
}

int DataFile::save_data(const char* formatted_str, const int led_pin){
  if (_file) {
    _file.println(formatted_str);
    _file.flush();
    Serial.print("Data saved: " );
    Serial.println(formatted_str);
    return SUCCESS;
  }
  else{
    Serial.println("Error opening file for writing!");
    return WRITE_ERR;
  }

}


//void ls(const File &dir){}