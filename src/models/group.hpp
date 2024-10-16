#pragma once

#include <boost/uuid/uuid.hpp>
#include <optional>
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/parse/boost_uuid.hpp"
#include "userver/formats/parse/common_containers.hpp"
#include "userver/formats/parse/to.hpp"
#include "userver/formats/serialize/common_containers.hpp"
#include "userver/formats/serialize/to.hpp"
#include "userver/storages/postgres/io/chrono.hpp"
#include <userver/formats/serialize/boost_uuid.hpp>


namespace models {
struct Group {
  std::optional<boost::uuids::uuid> id;
  std::optional<userver::storages::postgres::TimePointTz> created_at;
  std::optional<userver::storages::postgres::TimePointTz> updated_at;
  std::string group_name;
};

inline Group Parse(const userver::formats::json::Value& doc,
                   userver::formats::parse::To<Group>) {
  return {doc["id"].As<std::optional<boost::uuids::uuid>>(),
          doc["createdAt"]
              .As<std::optional<userver::storages::postgres::TimePointTz>>(),
          doc["updatedAt"]
              .As<std::optional<userver::storages::postgres::TimePointTz>>(),
          doc["groupName"].As<std::string>()};
}

inline userver::formats::json::Value Serialize(
    const Group& group,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;

  builder["id"] = group.id;
  builder["createdAt"] = group.created_at;
  builder["updatedAt"] = group.updated_at;
  builder["groupName"] = group.group_name;

  return builder.ExtractValue();
}

}  // namespace models