#include "utils/validator.hpp"
#include <gtest/gtest.h>

using namespace pawspective::utils;

TEST(ValidatorTest, EmailValidation) {
    EXPECT_TRUE(validate_email("test@example.com"));
    EXPECT_TRUE(validate_email("user.name+tag@domain.co.uk"));

    EXPECT_FALSE(validate_email("plainaddress"));
    EXPECT_FALSE(validate_email("@missinguser.com"));
    EXPECT_FALSE(validate_email("user@.com"));
}

TEST(ValidatorTest, PasswordStrength) {
    EXPECT_TRUE(validate_password_strength("Abc1234!"));

    EXPECT_FALSE(validate_password_strength("abc1234!"));
    EXPECT_FALSE(validate_password_strength("ABC1234!"));
    EXPECT_FALSE(validate_password_strength("Abcdefgh!"));
    EXPECT_FALSE(validate_password_strength("Abc12345"));
    EXPECT_FALSE(validate_password_strength("Ab1!"));
}

TEST(ValidatorTest, NotEmpty) {
    EXPECT_TRUE(validate_not_empty("Hello"));
    EXPECT_FALSE(validate_not_empty(""));
}