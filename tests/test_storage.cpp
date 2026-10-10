#include <gtest/gtest.h>
#include <string>
#include "Storage.h"

TEST(Storage, SetAndGet) {
    Storage<std::string> storage;
    storage.set("alex", "pereira");
    EXPECT_EQ(storage.get("alex"), "pereira");
}