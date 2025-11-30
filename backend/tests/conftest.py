import pytest_asyncio
import numpy as np
import pytest

from unittest.mock import MagicMock, patch
from httpx import ASGITransport, AsyncClient
from clear_vision.entrypoints.api.main import app


@pytest.fixture
def mock_video_capture():
    with patch("clear_vision.adapters.frame_samplers.cv2.VideoCapture") as mock_class:
        mock_cap = MagicMock()
        mock_class.return_value = mock_cap

        mock_cap.isOpened.return_value = True

        def configure(num_frames=10, fps=5, opened=True):
            mock_cap.isOpened.return_value = opened
            mock_cap.get.return_value = fps

            if opened is False:
                mock_cap.read.side_effect = None
                return

            frames = [
                (True, np.random.randint(0, 255, (240, 320, 3), dtype=np.uint8))
                for _ in range(num_frames)
            ] + [(False, None)]
            mock_cap.read.side_effect = frames

        mock_cap.configure = configure
        yield mock_cap


@pytest_asyncio.fixture
async def async_client():
    transport = ASGITransport(app=app)

    async with AsyncClient(
        transport=transport, base_url="http://0.0.0.0/clear-vision/v1"
    ) as client:
        yield client
