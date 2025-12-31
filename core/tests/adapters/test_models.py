from unittest.mock import MagicMock, patch
from clear_vision.adapters.models import LlavaInterleaveQwenModel
from tests.fakes import FakeChatbotModel, FakeHFChatbotModel


def test_chatbot_model_interface_initialization():
    instance = FakeChatbotModel()
    assert instance


@patch("transformers.utils.logging.set_verbosity_error")
def test_hf_chatbot_model_interface_initialization(mock_log):
    instance = FakeHFChatbotModel()
    mock_log.assert_called_once()
    assert instance


@patch("transformers.LlavaProcessor.from_pretrained")
@patch("transformers.LlavaForConditionalGeneration.from_pretrained")
def test_llava_interleave_qwen_model_initialization(mock_model, mock_processor):
    mock_model.return_value = MagicMock()
    mock_processor.return_value = MagicMock()

    instance = LlavaInterleaveQwenModel()

    assert instance.model is mock_model.return_value
    assert instance.processor is mock_processor.return_value

    mock_model.assert_called_once()
    called_args, called_kwargs = mock_model.call_args

    # Assert auto parameter to support GPU offload
    assert called_kwargs["device_map"] == "auto"