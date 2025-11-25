from unittest.mock import MagicMock, patch
from clear_vision.adapters.models import LlavaInterleaveQwenModel
from tests.fakes import FakeChatbotModel, FakeHFChatbotModel


def test_chatbot_model_interface_initialization():

    instance = FakeChatbotModel(model_id="xyz")
    assert instance.model_id == "xyz"
    assert instance.model == "fake-chatbot-xyz"


@patch("transformers.utils.logging.set_verbosity_error")
def test_hf_chatbot_model_interface_initialization(mock_log):

    instance = FakeHFChatbotModel(model_id="abc")
    assert instance.model_id == "abc"
    assert instance.model == "fake-hf-chatbot-abc"
    mock_log.assert_called_once()


@patch("transformers.LlavaProcessor.from_pretrained")
@patch("transformers.LlavaForConditionalGeneration.from_pretrained")
def test_llava_interleave_qwen_model_initialization(mock_model, mock_processor):
    mock_model.return_value = MagicMock()
    mock_processor.return_value = MagicMock()

    instance = LlavaInterleaveQwenModel(model_id="qwen-llava")

    assert instance.model_id == "qwen-llava"
    assert instance.model is mock_model.return_value
    assert instance.processor is mock_processor.return_value

    mock_model.assert_called_once()
    called_args, called_kwargs = mock_model.call_args

    assert called_args[0] == "qwen-llava"
    assert called_kwargs["device_map"] == "auto"
    assert "quantization_config" in called_kwargs
    assert "dtype" in called_kwargs

    mock_processor.assert_called_once_with("qwen-llava")
