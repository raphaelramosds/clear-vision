import os
import typing as t

from clear_vision.infra.dynamodb.connection import Connection
from clear_vision.domain.entities import Inference, Video
from clear_vision.interfaces.repositories import (
    InferenceRepositoryInterface,
    VideoRepositoryInterface,
)


class VideoDynamoDbRepository(VideoRepositoryInterface):

    def __init__(self):
        self.table = Connection.resource().Table(os.getenv("VIDEOS_TABLE", "Videos"))

    def add(self, video: Video) -> Video:
        item = video.model_dump()
        self.table.put_item(
            Item=item,
            ConditionExpression="attribute_not_exists(uid)",
        )
        return video

    def get(self, uid: str) -> t.Optional[Video]:
        resp = self.table.get_item(Key={"uid": uid})
        if "Item" not in resp:
            return None
        return Video(**resp["Item"])

    def get_all(self) -> t.Optional[t.List[Video]]:
        resp = self.table.scan()
        items = resp.get("Items", [])
        return [Video(**i) for i in items]


class InferenceDynamoDbRepository(InferenceRepositoryInterface):

    def __init__(self):
        self.table = Connection.resource().Table(os.getenv("INFERENCES_TABLE", "Inferences"))
        self.gsi_name = os.getenv("INFERENCES_GSI", "video_uid_idx")

    def add(self, inference: Inference) -> Inference:
        item = inference.model_dump()
        self.table.put_item(
            Item=item,
            ConditionExpression="attribute_not_exists(uid)",
        )
        return inference

    def get(self, uid: str) -> t.Optional[Inference]:
        resp = self.table.get_item(Key={"uid": uid})
        if "Item" not in resp:
            return None
        return Inference(**resp["Item"])

    def get_all(self) -> t.Optional[t.List[Inference]]:
        resp = self.table.scan()
        items = resp.get("Items", [])
        return [Inference(**i) for i in items]

    def get_where(
        self, where_dict: t.Dict[str, t.Union[str, int]]
    ) -> t.Optional[t.List[Inference]]:

        if "video_uid" in where_dict:
            value = where_dict["video_uid"]
            try:
                resp = self.table.query(
                    IndexName=self.gsi_name,
                    KeyConditionExpression="video_uid = :v",
                    ExpressionAttributeValues={":v": value},
                )
                items = resp.get("Items", [])
                return [Inference(**i) for i in items]
            except self.table.meta.client.exceptions.ResourceNotFoundException:
                pass

        filter_expr = []
        expr_values = {}
        for i, (key, value) in enumerate(where_dict.items()):
            placeholder = f":v{i}"
            filter_expr.append(f"{key} = {placeholder}")
            expr_values[placeholder] = value

        resp = self.table.scan(
            FilterExpression=" AND ".join(filter_expr),
            ExpressionAttributeValues=expr_values,
        )
        items = resp.get("Items", [])
        return [Inference(**i) for i in items]
