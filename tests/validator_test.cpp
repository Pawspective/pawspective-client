#include "validator.hpp"
#include <gtest/gtest.h>

using namespace pawspective::utils;
using pawspective::services::ValidationError;

TEST(ValidatorTest, EmailValidationAcceptsValidAddresses) {
    Validator validator;
    validator.field("email", "test@example.com").validateEmail();
    validator.field("backup_email", "user.name+tag@domain.co.uk").validateEmail();

    EXPECT_FALSE(validator.getValidationError().has_value());
}

TEST(ValidatorTest, EmailValidationRejectsInvalidAddresses) {
    Validator validator;
    validator.field("email", "plainaddress").validateEmail();
    validator.field("backup_email", "@missinguser.com").validateEmail();
    validator.field("contact_email", "user@.com").validateEmail();

    auto error = validator.getValidationError();
    ASSERT_TRUE(error.has_value());
    const auto& errors = error->getErrors();
    ASSERT_EQ(errors.size(), 3);

    EXPECT_EQ(errors[0].fieldName, "email");
    EXPECT_EQ(errors[1].fieldName, "backup_email");
    EXPECT_EQ(errors[2].fieldName, "contact_email");
    EXPECT_EQ(errors[0].errorMessage, "must be a valid email address");
}

TEST(ValidatorTest, PasswordStrengthValidation) {
    Validator validValidator;
    validValidator.field("password", "Abc1234!").validatePasswordStrength();
    EXPECT_FALSE(validValidator.getValidationError().has_value());

    Validator invalidValidator;
    invalidValidator.field("password1", "abc1234!").validatePasswordStrength();
    invalidValidator.field("password2", "ABC1234!").validatePasswordStrength();
    invalidValidator.field("password3", "Abcdefgh!").validatePasswordStrength();
    invalidValidator.field("password4", "Abc12345").validatePasswordStrength();
    invalidValidator.field("password5", "Ab1!").validatePasswordStrength();

    auto error = invalidValidator.getValidationError();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error->getErrors().size(), 5);
}

TEST(ValidatorTest, NotBlankValidation) {
    Validator validValidator;
    validValidator.field("title", "Hello").notBlank();
    EXPECT_FALSE(validValidator.getValidationError().has_value());

    Validator invalidValidator;
    invalidValidator.field("name", "").notBlank();
    invalidValidator.field("bio", "   \t\n").notBlank();

    auto error = invalidValidator.getValidationError();
    ASSERT_TRUE(error.has_value());
    const auto& errors = error->getErrors();
    ASSERT_EQ(errors.size(), 2);
    EXPECT_EQ(errors[0].errorMessage, "must not be empty or whitespace-only");
    EXPECT_EQ(errors[1].errorMessage, "must not be empty or whitespace-only");
}

TEST(ValidatorTest, MinAndMaxLengthValidation) {
    Validator validator;
    validator.field("username", "ab").minLength(3);
    validator.field("nickname", "abcdef").maxLength(5);

    auto error = validator.getValidationError();
    ASSERT_TRUE(error.has_value());
    const auto& errors = error->getErrors();
    ASSERT_EQ(errors.size(), 2);
    EXPECT_EQ(errors[0].errorMessage, "must be at least 3 characters long");
    EXPECT_EQ(errors[1].errorMessage, "must be at most 5 characters long");
}

TEST(ValidatorTest, IsOneOfValidation) {
    Validator validValidator;
    validValidator.field("role", "admin").isOneOf({"admin", "member", "guest"});
    EXPECT_FALSE(validValidator.getValidationError().has_value());

    Validator invalidValidator;
    invalidValidator.field("role", "owner").isOneOf({"admin", "member", "guest"});

    auto error = invalidValidator.getValidationError();
    ASSERT_TRUE(error.has_value());
    ASSERT_EQ(error->getErrors().size(), 1);
    EXPECT_EQ(error->getErrors()[0].fieldName, "role");
}

TEST(ValidatorTest, CollectsMultipleErrorsAcrossFields) {
    Validator validator;
    validator.field("email", "bad").validateEmail();
    validator.field("password", "short").validatePasswordStrength();
    validator.field("name", " ").notBlank();

    auto error = validator.getValidationError();
    ASSERT_TRUE(error.has_value());
    const auto& errors = error->getErrors();
    ASSERT_EQ(errors.size(), 3);

    EXPECT_EQ(errors[0].fieldName, "email");
    EXPECT_EQ(errors[1].fieldName, "password");
    EXPECT_EQ(errors[2].fieldName, "name");
}