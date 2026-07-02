@echo off
REM Telemetry schema validator and generator.
REM Usage:
REM   generate_telemetry validate telemetry_config_example.json
REM   generate_telemetry generate telemetry_config_example.json
REM   generate_telemetry generate telemetry_config_example.json --out-dir generatedconfigs

python telemetry_codegen.py %*
