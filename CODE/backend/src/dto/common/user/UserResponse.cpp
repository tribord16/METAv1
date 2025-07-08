/**
 * @file UserResponse.cpp
 * @brief Implémentation UserResponse
 */

#include "dto/user/UserResponse.h"

namespace dto {
namespace user {

UserResponse::UserResponse(const models::User& user)
    : id(user.id), username(user.username), email(user.email),
      is_active(user.is_active), created_at(user.getCreatedAtString()) {}

UserResponse UserResponse::fromUser(const models::User& user) {
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