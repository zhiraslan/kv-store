#include <gtest/gtest.h>
#include "Storage.h"
#include "Service.h"


TEST(Service, Set_EmptyKey_StatusInvalidKey) {
	Storage testStorage;
	Service testService{ testStorage };

	ASSERT_EQ(testService.set("", "Bonaparte"), Status::INVALID_KEY); // EXPECT_EQ - so that the test doesn't crash if fails
	std::cout << "EmptyKey is passed" << std::endl;

}
TEST(Service, Get_ValidKey_StatusOK) {
	Storage testStorage;
	Service testService{ testStorage };

	testService.set("Napoleon", "Bonaparte");
	auto res = testService.get("Napoleon");

	ASSERT_EQ(res, Result<std::string>{"Bonaparte"});
	std::cout << "ValidKey is passed" << std::endl << std::endl;;
}

TEST(Service, Set_TooLongKey_StatusInvalidKey) {
	Storage testStorage;
	Service testService{ testStorage };

	std::string longKey(257, 'a' );
	
	ASSERT_EQ(testService.set(longKey, "some value"), Status::INVALID_KEY);
	std::cout << "TooLongKey is passed" << std::endl;
}