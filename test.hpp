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


// struct TestDataFileList {

//   DataFileList* dfl;

//   TestDataFileList(){
//     this->dfl = new(DataFileList);  
//   };

//   ~TestDataFileList(){
//     delete this->dfl;
//   }

//   void test_ls();
// };

// void test_ls(){
  
//   assert false;
// }