#include <Arduino.h>
#include <time.h>
#include "datalog.h"
#include "error.h"

int sample_counter = 0;
File output_file;

String format_data(const float* piezoValues, const int len){
  // Format the data into a CSV-style string
  String data = String(millis()) + ',';
  for (int i = 0; i < len; i++) {
    data += String(piezoValues[i]);
    if (i < len - 1) data += ",";
  }
  return data;
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

//////// DataFileList methods ///////////////////
void DataFileManager::_find_next_index(const File &dir) {

  int numeric_indexes[_capacity];
  int count = 0;

  while(true){
    File entry = dir.openNextFile();
    if (!entry){
      break; //no more files
    }
    
    if (entry.isDirectory()) continue;

    if (strcmp("DATA", entry.name())==0) {
      // extract id and convert to int
      numeric_indexes[count++] = atoi(entry.name()+5); // the substing starting at position 5 should be the index 
      count++;
    }    
  }
  // let's sort the indexes first
  qsort(numeric_indexes, count, sizeof(int), _compare_ints);

  _index = numeric_indexes[count];
}
//////// DataFileManager related code ///////////////////

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





int DataFileManager::save_data(const float* piezoValues, const int num_values, const int led_pin) {
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


//void ls(const File &dir){}