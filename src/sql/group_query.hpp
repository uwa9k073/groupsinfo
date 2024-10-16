#pragma once

#include "userver/storages/postgres/query.hpp"
namespace sql {
const userver::storages::postgres::Query kCreateGroup{
    R"(
    INSERT INTO  groupsinfo_schema.groups (group_name, idempotency_key) 
    VALUES ($1, $2) RETURNING id;
)",
    userver::storages::postgres::Query::Name{"create-new-group"}};

const userver::storages::postgres::Query kTakeGroups{
    R"(
        SELECT id, created_at, updated_at, group_name
        FROM groupsinfo_schema.groups
)",
    userver::storages::postgres::Query::Name{"select-all-groups"}};

const userver::storages::postgres::Query kDeleteGroup{R"(
    DELETE FROM 
        groupsinfo_schema.groups 
    WHERE
        id = $1
)"};

const userver::storages::postgres::Query kUpdateGroup{
    R"(
        UPDATE
            groupsinfo_schema.groups
        SET
            group_name = $2,
            updated_at = now()
        WHERE
            id = $1
    )"
};
}  // namespace sql