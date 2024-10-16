#include "handler.hpp"
#include <optional>
#include <stdexcept>
#include "components/groups/component.hpp"
#include "exceptions/resource_not_found/exception.hpp"
#include "models/group.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/http/common_headers.hpp"
#include "userver/logging/log.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/server/http/http_request.hpp"
#include "userver/server/http/http_status.hpp"
#include "userver/server/request/request_context.hpp"
#include "userver/storages/postgres/exceptions.hpp"
#include "userver/utils/boost_uuid4.hpp"

constexpr userver::http::headers::PredefinedHeader kIdempotencyKey{
    "X-Idempotency-Key"};

namespace handlers::groups {
Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context),
      _groups_manager(context.FindComponent<components::groups::Component>()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& json,
    userver::server::request::RequestContext&) const {
  request.GetHttpResponse().SetHeader(
      std::string("Access-Control-Allow-Origin"), std::string("*"));
  request.GetHttpResponse().SetHeader(
      std::string("Access-Control-Allow-Headers"),
      std::string("Content-Type"));

  const auto& method = request.GetMethod();
  switch (method) {
    case userver::server::http::HttpMethod::kPost:
      return CreateGroup(request, json);
    case userver::server::http::HttpMethod::kGet:
      return TakeGroups(request);
    case userver::server::http::HttpMethod::kDelete:
      return DeleteGroup(request);
    case userver::server::http::HttpMethod::kPut:
      return UpdateGroup(request, json);
    default:
      throw ClientError(ExternalBody{
          fmt::format("Unsupported method {}", request.GetMethod())});
  }
}

userver::formats::json::Value Handler::CreateGroup(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& json) const {
  try {
    const auto& group = json.As<models::Group>();
    const auto& idempotency_key =
        userver::utils::BoostUuidFromString(request.GetHeader(kIdempotencyKey));
    const auto& id = _groups_manager.Create(group, idempotency_key);
    auto& response = request.GetHttpResponse();
    response.SetStatus(userver::server::http::HttpStatus::kCreated);

    response.SetHeader(userver::http::headers::kLocation,
                       fmt::format("/v1/groupsinfo/{}", id));
    return {};
  } catch (userver::storages::postgres::UniqueViolation& e) {
    LOG_WARNING() << e.what();
    return {};
  } catch (std::runtime_error& e) {
    LOG_ERROR() << e.what();
    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return {};
  }
}

userver::formats::json::Value Handler::TakeGroups(
    const userver::server::http::HttpRequest& request) const {

  try {
    return userver::formats::json::ValueBuilder{_groups_manager.TakeAll()}
        .ExtractValue();
  } catch (exceptions::ResourceNotFound& e) {
    LOG_WARNING() << e.what();
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return {};
  }
}

userver::formats::json::Value Handler::DeleteGroup(
    const userver::server::http::HttpRequest& request) const {
  try {
    const auto& id =
        userver::utils::BoostUuidFromString(request.GetPathArg("id"));

    _groups_manager.Delete(id);

    request.SetResponseStatus(userver::server::http::HttpStatus::kNoContent);

    return {};
  } catch (const exceptions::ResourceNotFound& e) {
    LOG_WARNING() << e.what();
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return {};
  } catch (const std::runtime_error& e) {
    LOG_WARNING() << e.what();
    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return {};
  }
}
userver::formats::json::Value Handler::UpdateGroup(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& json) const {
  try {
    const auto& id =
        userver::utils::BoostUuidFromString(request.GetPathArg("id"));

    const auto& name = json["name"].As<std::string>();
    _groups_manager.Update(id, name);
    return {};
  } catch (const exceptions::ResourceNotFound& e) {
    LOG_WARNING() << e.what();
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return {};
  } catch (const std::runtime_error& e) {
    LOG_WARNING() << e.what();
    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return {};
  }
}

}  // namespace handlers::groups