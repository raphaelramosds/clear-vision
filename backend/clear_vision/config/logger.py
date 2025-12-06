# -*- coding: utf-8 -*-
"""Define function for create logger."""

import logging

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler()],
)


def get_logger(name: str = __name__):
    return logging.getLogger(name=name)
