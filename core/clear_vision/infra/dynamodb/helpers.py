from clear_vision.config.logger import get_logger
from clear_vision.infra.dynamodb.connection import Connection

logger = get_logger(name=__name__)

base_client = Connection.client()
base_resource = Connection.resource()


def ddb_client_healthcheck():
    tables = base_client.list_tables().get("TableNames", [])
    logger.info(f"Found {len(tables)} tables. Connection seems to work.")


def ddb_start_tables():
    tables = base_client.list_tables().get("TableNames", [])
    logger.info(f"Found {len(tables)} tables")

    logger.info("Checking Videos table")
    if "Videos" not in tables:
        logger.info("Videos table not found. Creating table")
        base_client.create_table(
            TableName="Videos",
            KeySchema=[
                {"AttributeName": "uid", "KeyType": "HASH"},
            ],
            AttributeDefinitions=[
                {"AttributeName": "uid", "AttributeType": "S"},
            ],
            BillingMode="PAY_PER_REQUEST",
        )
        base_client.get_waiter("table_exists").wait(TableName="Videos")
    logger.info("Videos table created")

    logger.info("Checking Inferences table")
    if "Inferences" not in tables:
        logger.info("Inferences table not found. Creating table")
        base_client.create_table(
            TableName="Inferences",
            KeySchema=[
                {"AttributeName": "uid", "KeyType": "HASH"},
            ],
            AttributeDefinitions=[
                {"AttributeName": "uid", "AttributeType": "S"},
                {"AttributeName": "video_uid", "AttributeType": "S"},
            ],
            BillingMode="PAY_PER_REQUEST",
            # GSI
            GlobalSecondaryIndexes=[
                {
                    "IndexName": "video_uid_idx",
                    "KeySchema": [
                        {"AttributeName": "video_uid", "KeyType": "HASH"},
                    ],
                    "Projection": {"ProjectionType": "ALL"},
                }
            ],
        )
        base_client.get_waiter("table_exists").wait(TableName="Inferences")
    logger.info("Inferences table created")


def ddb_refresh_tables():
    tables = base_client.list_tables().get("TableNames", [])

    for table_name in ("Videos", "Inferences"):
        if table_name in tables:
            base_client.delete_table(TableName=table_name)

            waiter = base_client.get_waiter("table_not_exists")
            waiter.wait(TableName=table_name)

    ddb_start_tables()
