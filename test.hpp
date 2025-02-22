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
    SD.open("/tmp2/DATA3100.csv", FILE_WRITE);
    if(!SD.mkdir("tmp3")) Serial.println("----------------> E che cacchio!"); // empty folder to see if its will start from 0
    printDirectory(SD.open("/"), 0, 0, 2);
    Serial.println("done.");
    
  }

  void test_construction(){
    Serial.println("In TestDataFileManager.test_constructor: ");
    // test opening in root
    Serial.println("Test 1:");
    this->dfm = new DataFileManager("/");
    if (dfm->_index == 99){
      Serial.println("test 1 passed");
    }
    else {
      Serial.println("test 1 FAILED!");
    }
    if(this->dfm){
      delete this->dfm;
      this->dfm = nullptr;
    }

    Serial.println("Test 2:");
    // test opening in subdirectory
    this->dfm = new DataFileManager("tmp1");
    if (dfm->_index == 14){
      Serial.println("test 2 passed");
    }
    else {
      Serial.println("test 2 FAILED!");
    }
    if (this->dfm){
      delete this->dfm;
      this->dfm = nullptr;
    }

    Serial.println("Test 3:");
    // test index when folder is empty
    this->dfm = new DataFileManager("tmp3");
    if(dfm->_index == -1){
      Serial.println("test 3 passed. ");
    }
    else {
      Serial.println("test 3 FAILED!");
    }
    if (this->dfm){
      delete this->dfm;
      this->dfm = nullptr;
    }

    Serial.println("Test 4:");
    // test index when folder is empty
    this->dfm = new DataFileManager("tmp2");
    if(dfm->_index == 3100){
      Serial.println("test 4 passed. ");
    }
    else {
      Serial.println("test 4 FAILED!");
    }
    if (this->dfm){
      delete this->dfm;
      this->dfm = nullptr;
    }
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
    SD.remove("tmp2/DATA3100.csv");
    SD.rmdir("tmp2");
    SD.rmdir("tmp3");
    Serial.println("done.");
  }

  void test_ls();
};

// void test_ls(){
  
//   assert false;
// }