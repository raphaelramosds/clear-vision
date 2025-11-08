.PHONY: serve

serve:
	@poetry run uvicorn clear_vision.entrypoints.api.app:app --port 8000 --host 127.0.0.1 --reload

clean:
	rm -rf /tmp/*.mp4 /tmp/tmp*
	find . -type d -name "__pycache__" -exec rm -rf {} +