from uuid import uuid4

import pytest

ENDPOINT = "/v1/groupsinfo/"


async def test_create_group(service_client):
    response = await service_client.post(
        ENDPOINT,
        headers={"X-Idempotency-Key": str(uuid4())},
        json={"groupName": "IKB0-16-22"},
    )

    assert response.status == 201


@pytest.mark.pgsql("db_1", files=["initial_data.sql"])
async def test_take_groups(service_client):
    response = await service_client.get(ENDPOINT)

    assert len(response.json()) == 10


async def test_delete_group(service_client):
    response = await service_client.post(
        ENDPOINT,
        headers={"X-Idempotency-Key": str(uuid4())},
        json={"groupName": "IKB0-16-22"},
    )

    assert response.status == 201

    location = response.headers["Location"]

    response = await service_client.delete(location)

    assert response.status == 204


async def test_update_message(service_client):
    response = await service_client.post(
        ENDPOINT,
        headers={"X-Idempotency-Key": str(uuid4())},
        json={"groupName": "IKB0-16-22"},
    )

    assert response.status == 201

    location = response.headers["Location"]

    response = await service_client.put(location, json={"name": "IKBO-17-22"})

    assert response.status == 200
