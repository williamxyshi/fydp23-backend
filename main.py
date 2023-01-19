from fastapi import FastAPI
from pydantic import BaseModel, Field
import datetime

app = FastAPI()

class CreateBrew(BaseModel):
  type: str
  starttime: datetime
  endtime: datetime
  interval: datetime
  water_amount: int
  ground_amount: int

class EditBrew(BaseModel):
  id: str
  starttime: datetime
  endtime: datetime
  interval: datetime
  water_amount: int
  ground_amount: int

class DeleteBrew(BaseModel):
  id: str

#Return all brews
@app.get("/")
async def root():
    return {"message": "Hello World"}

#schedule a brew
@app.post("/")
async def root(data: CreateBrew):

    if data.type == 'single':
      print()

    elif data.type == 'schedule':
      print()

    return {"message": "Hello World"}

#edit a brew
@app.put("/")
async def root():
    return {"message": "Hello World"}

#delete a brew
@app.delete("/")
async def root():
    return {"message": "Hello World"}