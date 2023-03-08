from fastapi import FastAPI
from pydantic import BaseModel
import time
import math
import scheduler
import datetime

from consts import HRS_TO_SECS

app = FastAPI()

"""
brew types:
  - single -> immediately schedule a brew
  - schedule -> schedule a brew to occur on a set interval

single : starttime -> when it starts (could be now or future)
         endtime -> when to end the brew starting now

scheduled : starttime -> when it starts (could be now or future)
            endtime -> when to end the brew starting now
            interval -> every interval time we start a brew

"""


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


class Brew:
    ready_time: int
    duration: int


class State:
    def __init__(self):
        self.action: str = "wait"


class DeleteBrew(BaseModel):
    id: str


state = State()


@app.post("/brew/schedule")
async def scheduled_brew(data: ScheduledBrew):
    for day in data.days:
        time_stamp = data.ready_time.split(":")
        scheduler.add_brew_job(
            day=day, hour=time_stamp[0], minute=time_stamp[1], duration=data.duration
        )


# schedule a brew
@app.post("/brew/single")
async def single_brew(data: SingleBrew):
    start_time = datetime.datetime.fromtimestamp(
        data.ready_timestamp - data.duration * HRS_TO_SECS
    )
    scheduler.add_single_brew_job(start_time, data.duration)


@app.delete("/brew")
async def delete_brew():
    scheduler.remove_all_jobs()


@app.get("/action")
async def get_action():
    return {"action": state.action}


@app.get("/brews")
async def get_brews():
    return {"action": state.brews}
