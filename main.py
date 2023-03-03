from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import time
import math
import scheduler
from typing import Optional
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

HRS_TO_SECONDS = 3600

class SingleBrew(BaseModel):
    ready_timestamp: str
    duration: str
    
    water_amount: int
    ground_amount: int
    

class RepeatBrew(BaseModel):
    ready_time: str
    water_amount: int
    ground_amount: int
    days: list
    duration: str

class EditBrew(BaseModel):
    id: str
    start_time: str
    end_time: str
    interval: str
    water_amount: int
    ground_amount: int


class Demo(BaseModel):
    action: str


class Brew(BaseModel):
    ready_time: int
    duration: int


class Settings:
    def __init__(self):
        self.action: str = "wait"
        self.brews: list(Brew) = []

    def current_brew(self):
        if len(self.brews) == 0:
            return None
        else:
            return self.brews[0]

    def get_curr_brew_status(self):
        curr_brew = self.current_brew()
        if curr_brew:
            start_time = curr_brew.ready_time - curr_brew.duration * HRS_TO_SECONDS
            now = math.floor(time.time())
            print(start_time)
            print(now)
            if now < start_time:
                return "wait"
            elif now > curr_brew.ready_time:
                return "done"
            else:
                return "brewing"
        else:
            return "none"


class DeleteBrew(BaseModel):
    id: str


brews = []
current_brew = {}
settings = Settings()


# Return all brews
@app.get("/")
async def root():
    return {"data": brews}

@app.post("/brew/schedule")
async def repeated_brew(data: CreateBrew):
    print("schedule a brew")

    for day in data.days:
        time_stamp = data.start_time.split(":")
        print(time_stamp)
        scheduler.add_brew_job(day=day, hour=time_stamp[0], minute=time_stamp[1])   


# schedule a brew
@app.post("/brew")
async def root(data: CreateBrew):
    print("single brew")
    scheduler.add_single_brew_job(data.start_time)



# edit a brew
@app.put("/")
async def root():
    return {"message": "Hello World"}


# delete a brew
@app.delete("/")
async def root():
    return {"message": "Hello World"}


@app.post("/demo")
async def post_demo(data: Demo):
    print(data.action)
    print(type(data.action))
    if data.action != "go" and data.action != "stop":
        raise HTTPException(status_code=400, detail="action must be go or stop")
    settings.action = data.action
    return {"message": "success"}


@app.get("/demo")
async def get_demo():
    return {"action": settings.action}


@app.post("/brews")
async def post_brews(data: Brew):
    print(data)
    settings.brews.append(data)
    return {"message": "hey"}


@app.get("/brews")
async def get_brews():
    return {"brews": settings.brews}


@app.get("/current-brew-status")
async def get_current_brew_status():
    return {"current_brew": settings.get_curr_brew_status()}
