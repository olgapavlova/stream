#!/bin/sh
PYTHONWARNINGS='ignore:::pydantic_settings.sources.utils' \
exec eim run "idf.py mcp-server"
