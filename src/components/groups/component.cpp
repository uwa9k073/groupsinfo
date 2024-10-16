#include "component.hpp"
#include <string_view>
#include <vector>
#include "models/group.hpp"
#include "sql/group_query.hpp"
#include "userver/components/component_base.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include <exceptions/resource_not_found/exception.hpp>

namespace components::groups {
Component::Component(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context)
    : userver::components::ComponentBase(config, context),
      _pg_cluster(
          context.FindComponent<userver::components::Postgres>("pg-db-1")
              .GetCluster()) {}

boost::uuids::uuid Component::Create(
    const models::Group& group,
    const boost::uuids::uuid& idempotency_key) const {
  auto result = _pg_cluster->Execute(
      userver::storages::postgres::ClusterHostType::kMaster, sql::kCreateGroup,
      group.group_name, idempotency_key);

  return result.AsSingleRow<boost::uuids::uuid>();
}

std::vector<models::Group> Component::TakeAll() const {
  auto result = _pg_cluster->Execute(
      userver::storages::postgres::ClusterHostType::kSlave, sql::kTakeGroups);

  if (!result.RowsAffected()) {
    throw exceptions::ResourceNotFound();
  }

  return result.AsContainer<std::vector<models::Group>>(
      userver::storages::postgres::kRowTag);
}

void Component::Update(const boost::uuids::uuid& id, std::string_view name) const {
  auto result = _pg_cluster->Execute(
      userver::storages::postgres::ClusterHostType::kMaster, sql::kUpdateGroup,
      id, name);

  if (!result.RowsAffected()) {
    throw exceptions::ResourceNotFound();
  }
}

void Component::Delete(const boost::uuids::uuid& id) const {
  auto result = _pg_cluster->Execute(
      userver::storages::postgres::ClusterHostType::kMaster, sql::kDeleteGroup,
      id);

  if (!result.RowsAffected()) {
    throw exceptions::ResourceNotFound();
  }
}
}  // namespace components::groups