from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, BaseSettings
import datetime

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

class CreateBrew(BaseModel):
  type: str
  start_time: datetime.datetime
  end_time: datetime.datetime
  interval: datetime.datetime #every x hours
  water_amount: int
  ground_amount: int

class EditBrew(BaseModel):
  id: str
  start_time: datetime.datetime
  end_time: datetime.datetime
  interval: datetime.datetime
  water_amount: int
  ground_amount: int

class Demo(BaseModel):
  action: str

class Settings(BaseSettings):
   action: str

class DeleteBrew(BaseModel):
  id: str

brews = []
current_brew = {}
settings = Settings()

#Return all brews
@app.get("/")
async def root():
    return {"data": brews}

#schedule a brew
@app.post("/")
async def root(data: CreateBrew):

    if data.type == 'single':
      print("single brew")
      brew = {
        ""
      }

    elif data.type == 'schedule':
      print("schedule a brew")

    return {"message": "Hello World"}

#edit a brew
@app.put("/")
async def root():
    return {"message": "Hello World"}

#delete a brew
@app.delete("/")
async def root():
    return {"message": "Hello World"}

@app.post("/demo")
async def post_demo(data: Demo):
  if data.action != "go" or data.action != "stop":
    raise HTTPException(status_code=400, detail="action must be go or stop")
  settings.action = data.action
  return {"message": "success"}

@app.get("/demo")
async def get_demo():
   return {"action": settings.action}