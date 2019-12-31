

#include "catch2/catch.hpp"
#include <vector>

#define private public
#define protected public

#include "avDumpFormat.h"
#include "customExceptions.h"



TEST_CASE("simple test") {
  try {
    qtWidgetsInterface::avDumpFormat objectA("validUri", 0);
    REQUIRE(objectA.latestBufferPtr == nullptr);
  }  catch(const mediaSourceWrongException&) {
    INFO("All good");
  }  catch(...) {
    INFO("something went wrong");
    REQUIRE(1 == 0);
  }

  SECTION("not set test") {
    try {
      qtWidgetsInterface::avDumpFormat objectB("", 0);
      REQUIRE(objectB.latestBufferPtr == nullptr);
    } catch(const mediaSourceNotSetException&) {
      INFO("All good");
    } catch(...) {
      INFO("something went wrong");
      REQUIRE(1 == 0);
    }
  }
}

TEST_CASE("Need more tests") {
  INFO("Also need test content");
}

  

