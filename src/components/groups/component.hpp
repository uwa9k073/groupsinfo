#pragma once

#include <string_view>
#include <vector>
#include "models/group.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"

namespace components::groups {
class Component final : public userver::components::ComponentBase {
 public:
 constexpr static std::string_view kName{"component-groups"};
  Component(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);

  boost::uuids::uuid Create(const models::Group& group,
                            const boost::uuids::uuid& idempotency_key) const;

  std::vector<models::Group> TakeAll() const;

  void Update(const boost::uuids::uuid& id, std::string_view name) const;

  void Delete(const boost::uuids::uuid& id) const;

 private:
  userver::storages::postgres::ClusterPtr _pg_cluster;
};
}  // namespace components::groups