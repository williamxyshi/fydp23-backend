from pydantic import BaseModel


class SingleBrew(BaseModel):
    ready_timestamp: int
    duration: int
    water_amount: int
    ground_amount: int


class ScheduledBrew(BaseModel):
    ready_time: str
    water_amount: int
    ground_amount: int
    days: list
    duration: int


class Demo(BaseModel):
    action: str
