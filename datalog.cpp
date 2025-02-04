#include <Arduino.h>
#include <time.h>
#include "datalog.h"
#include "error.h"

int sample_counter = 0;
File output_file;

Sd2Card card;
SdVolume volume;
SdFile root;

void printDirectory(File dir, int numTabs) {

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }

    Serial.print(entry.name());

    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void StringList::ls(File dir){

  while(true){
    File entry = dir.openNextFile();
    if (!entry){
      break; //no more files
    }
    
    if (entry.isDirectory()) continue;

    this->push(entry.name());
  }
}

// FIXME: refactor to avoid code duplication
void DataFileList::ls(File dir) {
  while(true){
    File entry = dir.openNextFile();
    if (!entry){
      break; //no more files
    }
    
    if (entry.isDirectory()) continue;

    if (strcmp("DATA", entry.name())==0) {
      this->push(entry.name());
    }
      
  }
}

int SD_init(const int chipSelect) {
  if (!card.init(SPI_HALF_SPEED, chipSelect)) return SD_ERR_INIT;

  // check for SD info
  Serial.println();
  Serial.print("Card type:         ");
  switch (card.type()) {
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

  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }

  Serial.print("Clusters:          ");
  Serial.println(volume.clusterCount());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(volume.blocksPerCluster());
  Serial.print("Total Blocks:      ");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(volume.fatType(), DEC);
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);
  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  root.close();
  if (!SD.begin(chipSelect)) return SD_ERR_INIT;

  // check for eough free space
  File my_root;
  my_root = SD.open("/");
  printDirectory(my_root, 0);
  Serial.println("done!");

  return SD_OK;
}
  

String format_data(const float* piezoValues, const int len){
  // Format the data into a CSV-style string
  String data = String(millis()) + ',';
  for (int i = 0; i < len; i++) {
    data += String(piezoValues[i]);
    if (i < len - 1) data += ",";
  }
  return data;
}


int save_data(const float* piezoValues, const int num_values, const int led_pin) {
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