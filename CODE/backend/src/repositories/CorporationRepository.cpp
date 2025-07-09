#include "repositories/CorporationRepository.h"
#include "utils/Logger.h"

namespace repositories {

CorporationRepository::CorporationRepository(drogon::orm::DbClientPtr db) : db_(db) {}

void CorporationRepository::findByName(const std::string& name, std::function<void(std::optional<Corporations>)> callback) {
    auto mapper = drogon::orm::Mapper<Corporations>(db_);
    
    mapper.findBy(
        drogon::orm::Criteria(Corporations::Cols::_name, drogon::orm::CompareOperator::EQ, name),
        [callback](std::vector<Corporations> corps) {
            if (corps.empty()) {
                callback(std::nullopt);
            } else {
                callback(corps[0]);
            }
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            Logger::error("DB error: " + std::string(e.base().what()));
            callback(std::nullopt);
        }
    );
}

void CorporationRepository::create(const Corporations& corp,
                                      std::function<void(const Corporations&)> onSuccess,
                                      std::function<void(const std::string&)> onError) {
    Logger::info("[CorporationRepository] Creating corporation: " + corp.getValueOfName());

    auto mapper = drogon::orm::Mapper<Corporations>(db_);
    
    mapper.insert(
        corp,
        [onSuccess](const Corporations& createdCorp) {
            Logger::info("[CorporationRepository] Corporation created successfully");
            onSuccess(createdCorp);
        },
        [onError](const drogon::orm::DrogonDbException& e) {
            std::string errorMsg = "Failed to create corporation: " + std::string(e.base().what());
            Logger::error("[CorporationRepository] " + errorMsg);
            onError(errorMsg);
        }
    );
}

void CorporationRepository::findById(int id, std::function<void(std::optional<Corporations>)> callback) {
    auto mapper = drogon::orm::Mapper<Corporations>(db_);
    
    mapper.findByPrimaryKey(
        id,
        [callback](const Corporations& corp) {
            callback(corp);
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            Logger::error("DB error finding corporation by ID: " + std::string(e.base().what()));
            callback(std::nullopt);
        }
    );
}

void CorporationRepository::findByUser(int userId, std::function<void(const std::vector<Corporations>&)> callback) {
    auto mapper = drogon::orm::Mapper<Corporations>(db_);
    
    mapper.findBy(
        drogon::orm::Criteria(Corporations::Cols::_user_id, drogon::orm::CompareOperator::EQ, userId),
        [callback](std::vector<Corporations> corps) {
            callback(corps);
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            Logger::error("DB error: " + std::string(e.base().what()));
            callback(std::vector<Corporations>());
        }
    );
}

void CorporationRepository::update(const Corporations& corp,
                                   std::function<void(bool)> callback) {
    auto mapper = drogon::orm::Mapper<Corporations>(db_);
    
    mapper.update(
        corp,
        [callback](const size_t count) {
            if (count > 0) {
                Logger::info("[CorporationRepository] Corporation updated successfully");
                callback(true);
            } else {
                Logger::error("[CorporationRepository] No corporation was updated");
                callback(false);
            }
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            std::string errorMsg = "Failed to update corporation: " + std::string(e.base().what());
            Logger::error("[CorporationRepository] " + errorMsg);
            callback(false);
        }
    );
}

void CorporationRepository::remove(int corpId, std::function<void(bool)> callback) {
    auto mapper = drogon::orm::Mapper<Corporations>(db_);
    
    mapper.deleteByPrimaryKey(
        corpId,
        [callback](const size_t count) {
            if (count > 0) {
                Logger::info("[CorporationRepository] Corporation deleted successfully");
                callback(true);
            } else {
                Logger::error("[CorporationRepository] No corporation was deleted");
                callback(false);
            }
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            std::string errorMsg = "Failed to delete corporation: " + std::string(e.base().what());
            Logger::error("[CorporationRepository] " + errorMsg);
            callback(false);
        }
    );
}

} // namespace repositories
