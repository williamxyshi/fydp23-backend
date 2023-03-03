from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import time
import math

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


class CreateBrew(BaseModel):
    type: str
    start_time: str
    end_time: str
    interval: str  # every x hours
    water_amount: int
    ground_amount: int


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


# schedule a brew
@app.post("/")
async def root(data: CreateBrew):
    if data.type == "single":
        print("single brew")
        brew = {""}

    elif data.type == "schedule":
        print("schedule a brew")

    return {"message": "Hello World"}


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
