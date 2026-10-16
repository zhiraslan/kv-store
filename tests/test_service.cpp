#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Service.h"


TEST(Service, SetAndGetString) {
    Service service;
    service.set("alex", "pereira");
    EXPECT_EQ(service.get("alex"), "pereira");
}

TEST(Service, ValueExists) {
    Service service;
    auto result = service.get("non-existent");
    ASSERT_FALSE(result.has_value());
    service.set("non-existent", "existent");
    ASSERT_TRUE(service.get("non-existent").has_value());
}

TEST(Service, RewriteKey) {
    Service service;
    service.set("zhiraslan", "shedzhem");
    EXPECT_EQ(service.get("zhiraslan"), "shedzhem");
    service.set("zhiraslan", "GOAT");
    EXPECT_EQ(service.get("zhiraslan"), "GOAT");
}

TEST(Service, Deletion) {
    Service service;
    service.set("zhiraslan", "shedzhem");
    ASSERT_TRUE(service.get("zhiraslan").has_value());

    service.set("dina", "shedzhem");
    EXPECT_EQ(service.del({"zhiraslan", "dina"}), 2);
    ASSERT_FALSE(service.get("zhiraslan").has_value());

    EXPECT_EQ(service.del({"non-existent"}), 0);
}