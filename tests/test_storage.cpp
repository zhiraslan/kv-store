#include <gtest/gtest.h>
#include <string>
#include "Storage.h"

TEST(Storage, SetAndGetString) {
    Storage<std::string> storage;
    storage.set("alex", "pereira");
    EXPECT_EQ(storage.get("alex"), "pereira");
}

TEST(Storage, SetAndGetInt) {
    Storage<int> storage;
    storage.set("Zhiraslan", 7);
    EXPECT_EQ(storage.get("Zhiraslan"), 7);
}

TEST(Storage, ValueExists) {
    Storage<std::string> storage;
    auto result = storage.get("non-existent");
    ASSERT_FALSE(result.has_value());
    storage.set("non-existent", "existent");
    ASSERT_TRUE(storage.get("non-existent").has_value());
}