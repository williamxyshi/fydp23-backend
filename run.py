"""
This module is to run the fastapi webserver and is used
as a container entrypoint script
"""
import uvicorn

from settings import UvicornConfig
from os import getenv

# from service.core.database.database import engine

if __name__ == "__main__":
    uvicorn.run(
        UvicornConfig.FASTAPI_APP,
        host=UvicornConfig.WEBSERVER_HOST,
        port=UvicornConfig.WEBSERVER_PORT,
        reload=False,
        debug=UvicornConfig.DEBUG,
        workers=UvicornConfig.WORKERS_COUNT,
    )
