from fastapi import FastAPI

import scheduler
from state import State, Schedule
from models import SingleBrew, ScheduledBrew

app = FastAPI()

state = State()


@app.post("/brew/schedule")
async def scheduled_brew(data: ScheduledBrew):
    scheduler.remove_all_jobs()
    state.schedule = Schedule(data.days, data.ready_time)
    for day in data.days:
        await scheduler.add_scheduled_brew_job(
            day=day,
            ready_time=data.ready_time,
            duration=data.duration,
            strength=data.strength,
            size=data.size,
            is_cold=data.is_cold,
            state=state,
        )


@app.post("/brew/single")
async def single_brew(data: SingleBrew):
    await scheduler.add_single_brew_job(
        ready_timestamp=data.ready_timestamp,
        duration=data.duration,
        strength=data.strength,
        size=data.size,
        is_cold=data.is_cold,
        state=state,
    )


@app.delete("/brew")
async def delete_brew():
    scheduler.remove_all_jobs()


@app.get("/action")
async def get_action():
    return {
        "action": state.action,
        "num_dispenses": state.num_dispenses(),
        "pump_time": state.pump_time_ms(),
    }


@app.get("/start")
async def start():
    state.start_event.set()
    return {"message": "pinged successfully"}


@app.get("/finish")
async def finish():
    state.finish_event.set()
    return {"message": "pinged successfully"}


@app.get("/status")
async def get_brew_status():
    return {
        "start_timestamp": state.start_timestamp,
        "finish_timestamp": state.finish_timestamp,
        "duration": state.duration,
        "is_brewing": state.is_brewing,
        "is_done": state.is_done,
    }


@app.get("/brew/schedule")
async def get_schedule():
    return {
        "schedule": state.schedule,
    }
