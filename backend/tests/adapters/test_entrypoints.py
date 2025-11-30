import pytest


@pytest.mark.asyncio
async def test_upload_video(tmp_path, async_client):
    test_file = tmp_path / "fake.mp4"
    test_file.write_bytes(b"fake video bytes")

    with open(test_file, "rb") as f:
        response = await async_client.post(
            "/upload/",
            files={"video": ("fake.mp4", f, "video/mp4")},
        )
        assert response.status_code == 200

    json = response.json()
    assert json["message"] == "File uploaded"
    assert "file" in json
    assert "video_path" in json["file"]
    assert json["file"]["video_path"].endswith(".mp4")
