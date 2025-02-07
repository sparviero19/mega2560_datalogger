#include "datalog.h"


struct TestDataFileList {

  DataFileList* dfl;

  TestDataFileList(){
    this->dfl = new(DataFileList);  
  };

  ~TestDataFileList(){
    delete this->dfl;
  }

  void test_ls();
};

void test_ls(){
  
  assert false;
}