#include "datalog.h"
#include <SD.h>
#include <assert.h>


struct TestDataFile {

  DataFile* df;

  void test_construction(){
    Serial.println("Testing DataFile creation: ");
    delay(1000);
    df = new DataFile(42);
    if(SD.exists("DATA0042.csv")){
      delete df;
      SD.remove("DATA0042.csv");
      Serial.println("test passed.");
    }
    
    

    Serial.println("Testing DataFile creation with path:");
    SD.mkdir("tmp");
    df = new DataFile(89, "tmp");
    if(SD.exists("tmp/DATA0089.csv")){
      delete df;
      SD.remove("tmp/DATA0089.csv");
      SD.rmdir("tmp");
      Serial.println("test passed.");
    }
    
    

  }

  ~TestDataFile(){
    if (SD.exists("DATA0042.csv")) {
      Serial.println("removing DATA0042.csv ...");
      SD.remove("DATA0042.csv");
    }
    if (SD.exists("tmp/DATA0089.csv")){
      Serial.println("removing DATA0089.csv ...");
      SD.remove("tmp/DATA0089.csv");
      Serial.println("removing tmp/ ...");
      SD.remove("tmp");
    }
    Serial.println("test completed!");
  }
};


struct TestDataFileManager {

  DataFileManager* dfm = nullptr;
  const int chipSelect = 53;
  const int error_pin = 9;

  TestDataFileManager(){
    Serial.println("Creating test files...");
    // init test
    SD.open("DATA0001.csv", FILE_WRITE);
    SD.open("DATA0002.csv", FILE_WRITE);
    SD.open("DATA0005.csv", FILE_WRITE);
    SD.open("DATA0007.csv", FILE_WRITE);
    SD.open("DATA0014.csv", FILE_WRITE);
    SD.open("DATA0099.csv", FILE_WRITE);
    SD.mkdir("tmp1");
    SD.open("/tmp1/DATA0001.csv", FILE_WRITE);
    SD.open("/tmp1/DATA0002.csv", FILE_WRITE);
    SD.open("/tmp1/DATA0005.csv", FILE_WRITE);
    SD.open("/tmp1/DATA0007.csv", FILE_WRITE);
    SD.open("/tmp1/DATA0014.csv", FILE_WRITE);
    SD.mkdir("tmp2");
    SD.open("/tmp2/DATA0001.csv", FILE_WRITE);
    SD.open("/tmp2/DATA0002.csv", FILE_WRITE);
    SD.open("/tmp2/DATA0005.csv", FILE_WRITE);
    SD.open("/tmp2/DATA0007.csv", FILE_WRITE);
    SD.open("/tmp2/DATA0100.csv", FILE_WRITE);
    printDirectory(SD.open("/"), 0, 0, 2);
    Serial.println("done.");
    
  }

  void test_construction(){
    Serial.println("In TestDataFileManager.test_constructor: ");
    // test opening in root
    Serial.println("Test 1:");
    this->dfm = new DataFileManager("/", chipSelect, error_pin);
    if (dfm->_index == 99){
      Serial.println("test 1 passed");
    }
    if(this->dfm){
      delete this->dfm;
      this->dfm = nullptr;
    }

    Serial.println("Test 2:");
    // test opening in subdirectory
    this->dfm = new DataFileManager("tmp1", chipSelect, error_pin);
    if (dfm->_index == 14){
      Serial.println("test 2 passed");
    }
    if (this->dfm){
      delete this->dfm;
      this->dfm = nullptr;
    }

    // Serial.println("Test 3:");
    // // test index out of bounds
    // this->dfm = new DataFileManager("tmp2", chipSelect, error_pin);
    
  };

  ~TestDataFileManager(){
    Serial.println("in TestDataFileManager destructor");
    if (dfm){
      delete this->dfm;
    }
    Serial.println("Deleting test files...");
    // delete all test files
    SD.remove("DATA0001.csv");
    SD.remove("DATA0002.csv");
    SD.remove("DATA0005.csv");
    SD.remove("DATA0007.csv");
    SD.remove("DATA0014.csv");
    SD.remove("DATA0099.csv");
    SD.remove("tmp1/DATA0001.csv");
    SD.remove("tmp1/DATA0002.csv");
    SD.remove("tmp1/DATA0005.csv");
    SD.remove("tmp1/DATA0007.csv");
    SD.remove("tmp1/DATA0014.csv");
    SD.rmdir("tmp1");
    SD.remove("tmp2/DATA0001.csv");
    SD.remove("tmp2/DATA0002.csv");
    SD.remove("tmp2/DATA0005.csv");
    SD.remove("tmp2/DATA0007.csv");
    SD.remove("tmp2/DATA0100.csv");
    SD.rmdir("tmp2");
    Serial.println("done.");
  }

  void test_ls();
};

// void test_ls(){
  
//   assert false;
// }