#include "validator.hpp"
#include <regex>
#include <string>

namespace pawspective::utils {
bool validate_email(const std::string& email) {
    const std::regex email_regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, email_regex);
}

bool validate_password_strength(const std::string& password) {
    if (password.length() < 8) {
        return false;
    }
    bool has_upper = false;
    bool has_lower = false;
    bool has_digit = false;
    bool has_special = false;
    for (char c : password) {
        if (c >= 'A' && c <= 'Z') {
            has_upper = true;
        }
        if (c >= 'a' && c <= 'z') {
            has_lower = true;
        }
        if (c >= '0' && c <= '9') {
            has_digit = true;
        }
        if (c == '!' || c == '@' || c == '#' || c == '$' || c == '%' || c == '^' || c == '&' || c == '*') {
            has_special = true;
        }
    }
    return has_upper && has_lower && has_digit && has_special;
}

bool validate_not_empty(const std::string& value) { return !value.empty(); }
}  // namespace pawspective::utils