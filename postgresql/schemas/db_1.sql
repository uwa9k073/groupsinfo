DROP SCHEMA IF EXISTS groupsinfo_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS groupsinfo_schema;

CREATE TABLE IF NOT EXISTS groupsinfo_schema.groups (
    id uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    group_name TEXT NOT NULL,
    idempotency_key uuid UNIQUE
);
