"""Main configuration parameters for FastAPI"""

from os import getenv
from typing import List

from starlette.datastructures import CommaSeparatedStrings, Secret

# class FastApiConfig:
#     """FastAPI settings"""

#     API_PREFIX: str = getenv("API_PREFIX")
#     ALLOWED_HOSTS: List[str] = CommaSeparatedStrings(getenv('ALLOWED_HOSTS'))
#     DEBUG: bool = getenv('DEBUG', 'False') == 'True'
#     PROJECT_DESCRIPTION: str = f"{getenv('PROJECT_DESCRIPTION')} - Running on {getenv('HOSTNAME')}"
#     PROJECT_NAME: str = getenv('PROJECT_NAME')
#     VERSION: str = getenv('VERSION')


class UvicornConfig:
    """
    Uvicorn will first look for environment variables of the same name,
    or use the default values defined in this class.
    """

    DEBUG: bool = False
    FASTAPI_APP: str = getenv("FASTAPI_APP")
    # RELOAD: bool = getenv('RELOAD', 'False') == 'True'
    RELOAD: bool = False
    WEBSERVER_HOST: str = getenv("WEBSERVER_HOST")
    WEBSERVER_PORT: int = int(getenv("WEBSERVER_PORT", default=8080))
    WORKERS_COUNT: int = int(getenv("WORKERS_COUNT", default=1))
