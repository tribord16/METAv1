/**
 * @file UserResponse.cpp
 * @brief Implémentation UserResponse
 */

#include "dto/user/UserResponse.h"

namespace dto {
namespace user {

UserResponse::UserResponse(const Users& user)
    : id(user.getValueOfId()), username(user.getValueOfUsername()), email(user.getValueOfEmail()),
      is_active(user.getValueOfIsActive()), created_at(user.getValueOfCreatedAt().toFormattedStringLocal(false)) {}

UserResponse UserResponse::fromUser(const Users& user) {
    return UserResponse(user);
}

Json::Value UserResponse::toJson() const {
    Json::Value json;
    json["id"] = id;
    json["username"] = username;
    json["email"] = email;
    json["is_active"] = is_active;
    json["created_at"] = created_at;
    return json;
}

} // namespace user
} // namespace dto