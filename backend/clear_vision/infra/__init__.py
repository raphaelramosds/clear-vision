import os
import boto3

def bootstrap_dynamo():
    dynamo = boto3.client(
        "dynamodb",
        endpoint_url=os.getenv("DYNAMODB_ENDPOINT_URL"),
        region_name=os.getenv("AWS_REGION", "us-east-1"),
        aws_access_key_id=os.getenv("AWS_ACCESS_KEY_ID", "fake"),
        aws_secret_access_key=os.getenv("AWS_SECRET_ACCESS_KEY", "fake"),
    )

    tables = [t["TableName"] for t in dynamo.list_tables().get("TableNames", [])]

    if "Videos" not in tables:
        dynamo.create_table(
            TableName="Videos",
            KeySchema=[
                {"AttributeName": "uid", "KeyType": "HASH"},
            ],
            AttributeDefinitions=[
                {"AttributeName": "uid", "AttributeType": "S"},
            ],
            BillingMode="PAY_PER_REQUEST",
        )

    if "Inferences" not in tables:
        dynamo.create_table(
            TableName="Inferences",
            KeySchema=[
                {"AttributeName": "inference_uid", "KeyType": "HASH"},
            ],
            AttributeDefinitions=[
                {"AttributeName": "inference_uid", "AttributeType": "S"},
                {"AttributeName": "video_uid", "AttributeType": "S"},
            ],
            BillingMode="PAY_PER_REQUEST",
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

    print("DynamoDB tables ready.")
