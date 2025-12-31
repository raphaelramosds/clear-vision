import os

from botocore.config import Config as BotoConfig

class Config:

    ENDPOINT = os.getenv("DYNAMODB_ENDPOINT_URL")
    REGION = os.getenv("AWS_REGION", "us-east-1")
    ACCESS_KEY = os.getenv("AWS_ACCESS_KEY_ID", "fake")
    SECRET_KEY = os.getenv("AWS_SECRET_ACCESS_KEY", "fake")

    BOTO_CONFIG = BotoConfig(
        connect_timeout=2,
        read_timeout=2,
        retries={"max_attempts": 2},
    )

