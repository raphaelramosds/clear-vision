import boto3
from botocore.client import BaseClient
from clear_vision.infra.dynamodb.config import Config


class Connection:

    @classmethod
    def _boto_kwargs(cls):
        return {
            "endpoint_url": Config.ENDPOINT,
            "region_name": Config.REGION,
            "aws_access_key_id": Config.ACCESS_KEY,
            "aws_secret_access_key": Config.SECRET_KEY,
            "config": Config.BOTO_CONFIG,
        }

    @classmethod
    def client(cls) -> BaseClient:
        return boto3.client("dynamodb", **cls._boto_kwargs())

    @classmethod
    def resource(cls):
        return boto3.resource("dynamodb", **cls._boto_kwargs())
