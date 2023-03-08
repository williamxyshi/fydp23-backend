from pydantic import BaseModel


class SingleBrew(BaseModel):
    ready_timestamp: int
    size: str
    strength: str
    duration: int  # in seconds!


class ScheduledBrew(BaseModel):
    ready_time: str
    days: list
    size: str
    strength: str
    duration: int  # in seconds!


class Demo(BaseModel):
    action: str
