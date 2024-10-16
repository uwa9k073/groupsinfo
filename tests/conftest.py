import pathlib

import pytest
from testsuite.databases.pgsql import discover

pytest_plugins = ["pytest_userver.plugins.postgresql"]

USERVER_CONFIG_HOOKS = ["secdist_path"]


@pytest.fixture(scope="session")
def root_dir():
    """Path to root directory service."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope="session")
def secdist_path(root_dir):
    """Change path to secdist"""

    def patch_config(config, config_vars):
        components = config["components_manager"]["components"]
        components["default-secdist-provider"]["config"] = str(
            root_dir / "configs/secure_data.json"
        )

    return patch_config


@pytest.fixture(scope="session")
def pgsql_local(root_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        "groupsinfo",  # service name that goes to the DB connection
        [root_dir.joinpath("postgresql/schemas")],
    )
    return pgsql_local_create(list(databases.values()))
