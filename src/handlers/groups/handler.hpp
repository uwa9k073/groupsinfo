#pragma once

#include "components/groups/component.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/server/http/http_request.hpp"

namespace handlers::groups {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  constexpr static std::string_view kName{"handler-groups"};
  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& request_json,
      userver::server::request::RequestContext& context) const override;

 private:
  const ::components::groups::Component& _groups_manager;
  userver::formats::json::Value CreateGroup(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json) const;
  userver::formats::json::Value TakeGroups(
      const userver::server::http::HttpRequest& request) const;
  userver::formats::json::Value DeleteGroup(
      const userver::server::http::HttpRequest& request) const;
  userver::formats::json::Value UpdateGroup(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json) const;
};
}  // namespace handlers::groups